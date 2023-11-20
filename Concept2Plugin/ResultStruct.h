typedef struct _resultStruct
{
	// 0: Unititialized, 1: Initialized, 2 : data available
	int validityStatus;

	// The time of measurement (in seconds)
	float measurementTime;

	// The distance passed (in meters)
	float measurementDistance;

	// The velocity we have passed.
	float velocity;

	// 0: inactive 1 : active
	unsigned char rowingState;

	// STROKESTATE_WAITING_FOR_WHEEL_TO_REACH_MIN_SPEED_STATE,
	// STROKESTATE_WAITING_FOR_WHEEL_TO_ACCELERATE_STATE,
	//	STROKESTATE_DRIVING_STATE,
	//	STROKESTATE_DWELLING_AFTER_DRIVE_STATE,
	//	STROKESTATE_RECOVERY_STATE
	unsigned char strokeState;
} ResultStruct;
