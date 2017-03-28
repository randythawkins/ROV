int ReadCameraPotPan()
{
    return limitMinMaxSliderPotValues(analogRead(CAMERA_POT_PAN));
}

int ReadCameraPotTilt()
{
    return limitMinMaxSliderPotValues(analogRead(CAMERA_POT_TILT));
}

int limitMinMaxSliderPotValues( int value)
{
    return map(value, 1, 1024, 0, 180);
}

