int readCameraPanPot()
{
    return limitMinMaxSliderPotValues(analogRead(A0));
}

int readCameraTiltPot()
{
    return limitMinMaxSliderPotValues(analogRead(A1));
}

// Our pots should give us between 1 and 1023.  
//  Actual measurements showed 2 to about 1006. 
//  For a standard we will limit to between the defined values.
int limitMinMaxSliderPotValues( int value)
{
//    return constrain(value, MAP_SERVO_POT_LOWER_LIMIT, MAP_SERVO_POT_UPPER_LIMIT);

    return map(value, MAP_SERVO_POT_LOWER_LIMIT, MAP_SERVO_POT_UPPER_LIMIT, 0, 180);

}
