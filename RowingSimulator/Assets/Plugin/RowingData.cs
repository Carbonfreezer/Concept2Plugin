

using System;
using System.Runtime.InteropServices;

namespace Assets.Plugin
{
    /// <summary>
    /// Contains all the information for the rowing data.
    /// </summary>
    public class RowingData
    {
        /// <summary>
        /// The enumeration indicating if we have established contact with the heart rate sensor.
        /// </summary>
        public enum ConnectionStatus
        {
            Disconnected,
            Connected,
            DataAvailable
        }

        /// <summary>
        /// Contains the stroke data from the api. Meaning is yet to be determined.
        /// </summary>
        public enum StrokeStatus
        {
            WaitingForWheelToReachMinSpeedState, 
            WaitingForWheelToAccelerateState,
            DrivingState,
            DwellingAfterDriveState,
            RecoveryState
        }


        /// <summary>
        /// Do we get data from the concept 2 ergometer.
        /// </summary>
        public ConnectionStatus CurrentConnection { get;  }

        /// <summary>
        /// The time passed since exercise start.
        /// </summary>
        public float Time { get; }

        /// <summary>
        /// The distance passed since exercise start.
        /// </summary>
        public float Distance { get; }

        /// <summary>
        /// The current velocity we have.
        /// </summary>
        public float Velocity { get; }

        /// <summary>
        /// Are we currently rowing?
        /// </summary>
        public bool IsRowing { get; }

        /// <summary>
        /// The current stroke state we have.
        /// </summary>
        public StrokeStatus MovementState { get; }


        /// <summary>
        /// Dummy constructor from no data.
        /// </summary>
        public RowingData()
        {
            CurrentConnection = ConnectionStatus.Disconnected;
        }

        /// <summary>
        /// Constructor from native data.
        /// </summary>
        /// <param name="nativeData">Pointer from native data obtained from the cell.</param>
        public RowingData(IntPtr nativeData)
        {
            RowingInternal rawData = Marshal.PtrToStructure<RowingInternal>(nativeData);

            CurrentConnection = (ConnectionStatus)rawData.validityStatus;
            Time = rawData.measurementTime;
            Distance = rawData.measurementDistance;
            Velocity = rawData.velocity;
            IsRowing = (rawData.rowingState == 1);
            MovementState = (StrokeStatus)rawData.strokeState;
        }
    }
}