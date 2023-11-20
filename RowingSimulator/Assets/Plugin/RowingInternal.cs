using System.Runtime.InteropServices;

namespace Assets.Plugin
{
    /// <summary>
    ///     The data structure that mirrors the structure on the c++ side.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct RowingInternal
    {
        // 0: Unititialized, 1: Initialized, 2 : data available
        public int validityStatus;

        // The time of measurement (in seconds)
        public float measurementTime;

        // The distance passed (in meters)
        public float measurementDistance;

        // The velocity we have passed.
        public float velocity;

        // 0: inactive 1 : active
        public byte rowingState;

        // STROKESTATE_WAITING_FOR_WHEEL_TO_REACH_MIN_SPEED_STATE,
        // STROKESTATE_WAITING_FOR_WHEEL_TO_ACCELERATE_STATE,
        //	STROKESTATE_DRIVING_STATE,
        //	STROKESTATE_DWELLING_AFTER_DRIVE_STATE,
        //	STROKESTATE_RECOVERY_STATE
        public byte strokeState;
    }
}