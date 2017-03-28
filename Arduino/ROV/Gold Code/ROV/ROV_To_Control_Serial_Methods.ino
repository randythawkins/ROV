String GetROVData()
{

    return "";
}

// todo Do I need to continue to return a value from this function?
bool GetCommandFromControl()
{
bool bret = false;

    //LogMsg(F("Begin GetCommandFromControl()"));

    if( ET_GetCommandFromControl.receiveData())
    {
        LogMsg(F("GetCommandFromControl() Got Command from Control:"));
        LogMsg(ControlCommandToString());

        ParseCommandFromControl();
        bret = true;
    }
    
    //LogMsg("End GetCommandFromControl(); returning " + String(bret));
    return bret;
}

void ParseCommandFromControl()
{
bool localDebug = true;
    
    if( localDebug) LogMsg(F("Begin ParseCommandFromControl()"));
    
    switch (CommandFromControl.command)
    {
        case CMD_NO_COMMAND:
            if( localDebug) LogMsg(F("- CommandFromControl.command is CMD_NO_COMMAND"));
            break;
        case CMD_MOVECAMERA:
            if( localDebug) LogMsg(F("- CommandFromControl.command is CMD_MOVECAMERA"));

            if( CommandFromControl.cameraMoveMethod)    // CMD_CAMERAMOVEMETHOD_SET_POSITION (TRUE)
            {
                cameraPositionData.panPos = LimitCameraPanPositionValue(CommandFromControl.cameraPanPos);
                cameraPositionData.tiltPos = LimitCameraTiltPositionValue(CommandFromControl.cameraTiltPos);
            }
            else                                        // CMD_CAMERAMOVEMETHOD_DIRECTION_INCREMENT (FALSE)
            {
                cameraPositionData.panPos = LimitCameraPanPositionValue(
                        DetermineNewCameraPosition( GetCameraCurrentPanPosition(), 
                                                    CommandFromControl.cameraPanDirection, 
                                                    CommandFromControl.cameraPanIncrement));

                cameraPositionData.tiltPos = LimitCameraTiltPositionValue(
                        DetermineNewCameraPosition( GetCameraCurrentTiltPosition(), 
                                            CommandFromControl.cameraTiltDirection, 
                                            CommandFromControl.cameraTiltIncrement));
            }
            break;
        case CMD_GETROVDATA:
            if( localDebug) LogMsg(F("- CommandFromControl.command is CMD_GETROVDATA"));
            break;
        default:  
            if( localDebug) LogMsg("- Unrecognized command value" + String(CommandFromControl.command));
            break;
    }
    
    if( localDebug) LogMsg(F("End ParseCommandFromControl()"));
}

int DetermineNewCameraPosition(int _currentPosition, int _direction, int _increment)
{
int nret = CMD_CAMERA_SET_POSTION_DEFAULT;

LogMsg("_direction = " + String(_direction));
LogMsg("CMD_CAMERA_DIRECTION_PAN_RIGHT = " + String(CMD_CAMERA_DIRECTION_PAN_RIGHT));
LogMsg("CMD_CAMERA_DIRECTION_TILT_UP = " + String(CMD_CAMERA_DIRECTION_TILT_UP));
    if( (_direction == CMD_CAMERA_DIRECTION_PAN_RIGHT) || (_direction == CMD_CAMERA_DIRECTION_TILT_UP))                     
    {
        LogMsg("** decrement");                
        nret = _currentPosition -= _increment;
    }
    else                                        // CMD_CAMERA_DIRECTION_PAN_LEFT or CMD_CAMERA_DIRECTION_TILT_DOWN
    {
        LogMsg("** increment");                
        nret = _currentPosition += _increment;
    }
    
    return nret;
}

// todo will I need to keep this function?
void SendReadyResponseToControl()
{
    //LogMsg(F("Begin SendReadyResponseToControl()"));

    ClearResponseToControlDataStructure();
    
    ResponseToControl.response = RSP_ROV_READY;
    PopulateResponseDataElements();
    
    SendResponseToControl();
    
    //LogMsg(F("End SendReadyResponseToControl()"));
}

void SendResponseToControl()
{
static int msgCount = 1;

    LogMsg(F("Begin SendResponseToControl():"));
    LogMsg(ROVResponseToString());
        
    ResponseToControl.msgCount = msgCount;
    ET_SendResponseToComandToControl.sendData();
    msgCount++;
    
    LogMsg(F("End SendResponseToControl()"));
}

void PopulateResponseDataElements()
{
    ResponseToControl.cameraCurrentPanPos   = GetCameraCurrentPanPosition();
    ResponseToControl.cameraCurrentTiltPos  = GetCameraCurrentTiltPosition();
}

void ClearResponseToControlDataStructure()
{
    ResponseToControl.msgCount              = -1;
    ResponseToControl.response              = RSP_NO_RESPONSE;
    ResponseToControl.cameraCurrentPanPos   = CMD_CAMERA_SET_POSTION_DEFAULT;
    ResponseToControl.cameraCurrentTiltPos  = CMD_CAMERA_SET_POSTION_DEFAULT;
    memset( ResponseToControl.note, '\0', ET_NOTE_LEN + 1);
}

String ROVResponseToString()
{
String sret = "";
String linePrefix = "  - ";
String linePostfix = "\r\n";

    sret += "ResponseToControl" + linePostfix;
    sret += linePrefix + "msgCount: " + String(ResponseToControl.msgCount) + linePostfix;
    sret += linePrefix + "response: " + String(ResponseToControl.response) + linePostfix;
    sret += linePrefix + "cameraCurrentPanPos: " + String(ResponseToControl.cameraCurrentPanPos) + linePostfix;
    sret += linePrefix + "cameraCurrentTiltPos: " + String(ResponseToControl.cameraCurrentTiltPos) + linePostfix;
    sret += linePrefix + "note: '" + String(ResponseToControl.note) + "'" + linePostfix;
    
    return sret;
}

String ControlCommandToString()
{
String sret = "";
String linePrefix = "  - ";
String linePostfix = "\r\n";

    sret += "CommandFromControl" + linePostfix;
    sret += linePrefix + "msgCount: " + String(CommandFromControl.msgCount) + linePostfix;
    sret += linePrefix + "command: " + String(CommandFromControl.command) + linePostfix;
    sret += linePrefix + "cameraMoveMethod: " + String(CommandFromControl.cameraMoveMethod) + linePostfix;
    sret += linePrefix + "cameraPanPos: " + String(CommandFromControl.cameraPanPos) + linePostfix;
    sret += linePrefix + "cameraTiltPos: " + String(CommandFromControl.cameraTiltPos) + linePostfix;
    sret += linePrefix + "cameraPanDirection: " + String(CommandFromControl.cameraPanDirection) + linePostfix;
    sret += linePrefix + "cameraPanIncrement: " + String(CommandFromControl.cameraPanIncrement) + linePostfix;
    sret += linePrefix + "cameraTiltDirection: " + String(CommandFromControl.cameraTiltDirection) + linePostfix;
    sret += linePrefix + "cameraTiltIncrement: " + String(CommandFromControl.cameraTiltIncrement) + linePostfix;
    
    return sret;
}

