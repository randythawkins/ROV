void LogMsg( String msg)
{
    Serial.println(msg);    
    // Todo - add writing of msg to SD card log file
}

void LogMsg( byte msg)
{
    Serial.write(msg);    
    // Todo - add writing of msg to SD card log file
}
