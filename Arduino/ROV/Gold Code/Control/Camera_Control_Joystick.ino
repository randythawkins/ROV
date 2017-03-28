int ReadCameraJoystickPan()
{
    return limitMinMaxSliderPotValues(analogRead(CAMERA_JOYSTICK_PAN));
}

int ReadCameraJoystickTilt()
{
    return limitMinMaxSliderPotValues(analogRead(CAMERA_JOYSTICK_TILT));
}

int ReadCameraJoystickButton()
{
    return digitalRead(CAMERA_JOYSTICK_BUTTON);
}

int limitMinMaxJoystickPotValues( int value)
{
    return map(value, 1, 1024, 0, 180);
}
