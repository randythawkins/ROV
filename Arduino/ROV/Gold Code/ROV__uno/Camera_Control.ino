void CycleCamera()
{
int i;

    CenterCamera();
    //delay(2000);
    
    for( i = CAMERA_PAN_LOWER_LIMIT; i <= CAMERA_PAN_UPPER_LIMIT; i += 2)
        MoveCamera( i, CAMERA_TILT_CENTER_POSITION);
    
    for( i = CAMERA_PAN_UPPER_LIMIT; i >= CAMERA_PAN_LOWER_LIMIT; i -= 2)
        MoveCamera( i, CAMERA_TILT_CENTER_POSITION);
    //delay(2000);
        
    CenterCamera();
    //delay(2000);
    
    for( i = CAMERA_TILT_LOWER_LIMIT; i <= CAMERA_TILT_UPPER_LIMIT; i += 2)
        MoveCamera( CAMERA_PAN_CENTER_POSITION, i);
    
    for( i = CAMERA_TILT_UPPER_LIMIT; i >= CAMERA_TILT_LOWER_LIMIT; i -= 2)
        MoveCamera( CAMERA_PAN_CENTER_POSITION, i);
    //delay(2000);

    CenterCamera();
}

void CenterCamera()
{
    MoveCamera(CAMERA_PAN_CENTER_POSITION, CAMERA_TILT_CENTER_POSITION);
}

void MoveCamera(int pan, int tilt)
{
    LogMsg("Begin MoveCamera(); pan = " + String(pan) + ", tilt = " + String(tilt));
    
    PanServo.write(limitCameraPanPositionValue(pan)); 
    TiltServo.write(limitCameraTiltPositionValue(tilt)); 

    LogMsg("End MoveCamera()");
}

int limitCameraPanPositionValue( int value)
{
    return constrain(value, CAMERA_PAN_LOWER_LIMIT, CAMERA_PAN_UPPER_LIMIT);
}

int limitCameraTiltPositionValue( int value)
{
    return constrain(value, CAMERA_TILT_LOWER_LIMIT, CAMERA_TILT_UPPER_LIMIT);
}

