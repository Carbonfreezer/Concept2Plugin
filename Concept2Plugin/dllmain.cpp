#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Devices.Bluetooth.GenericAttributeProfile.h>
#include <winrt/Windows.Storage.Streams.h>


#include <mutex>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Devices::Bluetooth::GenericAttributeProfile;
using namespace Windows::Storage::Streams;

using namespace std;


#include "ResultStruct.h"
#include "FloatingAverager.h"


// The timeout when we assume data interruption (ms). 
const ULONGLONG timeOut = 10000L;

// The UUIDs we use.
const guid RowingService = guid("CE060030-43E5-11E4-916C-0800200C9A66");
const guid RowingNotifyCharacteristic = guid("CE060031-43E5-11E4-916C-0800200C9A66");
const guid rowingUpdateStatusCharacteristic = guid("CE060034-43E5-11E4-916C-0800200C9A66");




/*
    Data filled by callback.
*/


ResultStruct currentResult;
ResultStruct returnResult;
ULONGLONG lastReading;
bool isScanning{ false };
mutex dataLock;




/*
    Data needed for revoker.
*/


GattCharacteristic gRowingCallback{ nullptr };
GattCharacteristic::ValueChanged_revoker gBaseRevoker;

GattDeviceService gService{ nullptr };


/**
 * \brief We average the velocity over some elements (to nervous).
 */
FloatingAverager gVelocityBuffer(20);


void RowingData_ValueChanged(GattCharacteristic const& characteristic, GattValueChangedEventArgs args)
{
    lock_guard guard(dataLock);


    IBuffer buffer = args.CharacteristicValue();
    int  size = buffer.Length();
    assert(size == 19); // Unexpected payload size
    bool oldTimeAndDistanceValid = false;
    float oldTime;
    float oldDistance;
    if (currentResult.validityStatus == 2)
    {
        oldTimeAndDistanceValid = true;
        oldTime = currentResult.measurementTime;
        oldDistance = currentResult.measurementDistance;
    }
    currentResult.validityStatus = 2;

    const unsigned char* data = buffer.data();
    currentResult.measurementTime =  static_cast<float> (data[0] + (data[1] << 8) + (data[2] << 16)) * 0.01f;
    currentResult.measurementDistance = static_cast<float> (data[3] + (data[4] << 8) + (data[5] << 16)) * 0.1f;
    const float localVelocity = oldTimeAndDistanceValid  && (currentResult.measurementTime > oldTime + 0.01f) ?
	                                (currentResult.measurementDistance - oldDistance) / (currentResult.measurementTime - oldTime) : 0.0f;

    currentResult.velocity = gVelocityBuffer.NewElement(localVelocity);

    currentResult.rowingState = data[9];
    currentResult.strokeState = data[10];

    lastReading = GetTickCount64();

   
}



fire_and_forget StartScanning()
{
    // If we have a remaining inner connection we kill it. 
    isScanning = true;
    currentResult.validityStatus = 0;
    if (gService != nullptr)
    {
        gService.Close();
        gService = nullptr;
    }

    // Find the device with the rowing service.
    DeviceInformationCollection devices = co_await DeviceInformation::FindAllAsync(GattDeviceService::GetDeviceSelectorFromUuid(RowingService));
    if (devices.Size() == 0)
    {
        isScanning = false;
        co_return;
    }

    DeviceInformation targetDevice = devices.GetAt(0);
    // Get the services.
    gService = co_await GattDeviceService::FromIdAsync(targetDevice.Id());
    if (gService == nullptr)
    {
        isScanning = false;
        co_return;
    }

    
    // Get the characteristic for the rowing reading update.
   GattCharacteristicsResult charResult = co_await gService.GetCharacteristicsForUuidAsync(rowingUpdateStatusCharacteristic);
    if (charResult.Status() != GattCommunicationStatus::Success)
    {
        isScanning = false;
        co_return;
    }

    GattCharacteristic rowingWrite = charResult.Characteristics().GetAt(0);
    // For 100 ms
    const DataWriter writer;
    writer.WriteByte(3);
    const GattCommunicationStatus writeResult = co_await rowingWrite.WriteValueAsync(writer.DetachBuffer());
    if (writeResult != GattCommunicationStatus::Success)
    {
        isScanning = false;
        co_return;
    }
	    
   
    // Get the rowing information characteristic.
	charResult = co_await gService.GetCharacteristicsForUuidAsync(RowingNotifyCharacteristic);
    if (charResult.Status() != GattCommunicationStatus::Success)
    {
        isScanning = false;
        co_return;
    }


    // Get the characteristics for the rowing.
    gRowingCallback = charResult.Characteristics().GetAt(0);   

    GattCommunicationStatus status = co_await gRowingCallback.WriteClientCharacteristicConfigurationDescriptorAsync(GattClientCharacteristicConfigurationDescriptorValue::Notify);
    if (status != GattCommunicationStatus::Success)
    {
        isScanning = false;
        co_return;
    }
    gBaseRevoker = gRowingCallback.ValueChanged(auto_revoke, &RowingData_ValueChanged);
    currentResult.validityStatus = 1;
    lastReading = GetTickCount64();
    isScanning = false;
   

    co_return;
}




/*
    DLL Interface.
*/


extern "C" void __declspec(dllexport) * GetData()
{
    lock_guard data(dataLock);
    returnResult = currentResult;

    // Eventually we have to start the search 
    if (!isScanning)
    {
        if ((currentResult.validityStatus == 0) || (GetTickCount64() - lastReading > timeOut))
            StartScanning();
    }

    return &returnResult;
}


extern "C" void __declspec(dllexport) Disconnect()
{
    lock_guard data(dataLock);

    currentResult.validityStatus = 0;
    if (gService != nullptr)
    {
        gService.Close();
        gService = nullptr;
    }
}

