void CycleCamera()
{
int i;
const int DELAY_TIME = 200;
const int STEP_INCREMENT = 10;

    CenterCamera();
    delay(DELAY_TIME);
    
    for( i = CAMERA_PAN_LOWER_LIMIT; i <= CAMERA_PAN_UPPER_LIMIT; i += STEP_INCREMENT)
        MoveCamera( i, CAMERA_TILT_CENTER_POSITION);
    
    for( i = CAMERA_PAN_UPPER_LIMIT; i >= CAMERA_PAN_LOWER_LIMIT; i -= STEP_INCREMENT)
        MoveCamera( i, CAMERA_TILT_CENTER_POSITION);
    delay(DELAY_TIME);
        
    CenterCamera();
    delay(DELAY_TIME);
    
    for( i = CAMERA_TILT_LOWER_LIMIT; i <= CAMERA_TILT_UPPER_LIMIT; i += STEP_INCREMENT)
        MoveCamera( CAMERA_PAN_CENTER_POSITION, i);
    
    for( i = CAMERA_TILT_UPPER_LIMIT; i >= CAMERA_TILT_LOWER_LIMIT; i -= STEP_INCREMENT)
        MoveCamera( CAMERA_PAN_CENTER_POSITION, i);
    delay(DELAY_TIME);

    CenterCamera();
}

void CenterCamera()
{
    MoveCamera(CAMERA_PAN_CENTER_POSITION, CAMERA_TILT_CENTER_POSITION);
}

void CheckAndMoveCamera()
{
    if((cameraPositionData.panPos != GetCameraCurrentPanPosition()) || 
       (cameraPositionData.tiltPos != GetCameraCurrentTiltPosition()))
        MoveCamera( cameraPositionData.panPos, cameraPositionData.tiltPos);
}

void MoveCamera(int pan, int tilt)
{
    LogMsg("Begin MoveCamera(); pan = " + String(pan) + ", tilt = " + String(tilt));
    
    PanServo.write(LimitCameraPanPositionValue(pan)); 
    delay(10);
    TiltServo.write(LimitCameraTiltPositionValue(tilt)); 
    delay(10);

    LogMsg("End MoveCamera()");
}

int LimitCameraPanPositionValue( int value)
{
    return constrain(value, CAMERA_PAN_LOWER_LIMIT, CAMERA_PAN_UPPER_LIMIT);
}

int LimitCameraTiltPositionValue( int value)
{
    return constrain(value, CAMERA_TILT_LOWER_LIMIT, CAMERA_TILT_UPPER_LIMIT);
}

int GetCameraCurrentPanPosition()
{
    return PanServo.read();
}

int GetCameraCurrentTiltPosition()
{
    return TiltServo.read();
}

