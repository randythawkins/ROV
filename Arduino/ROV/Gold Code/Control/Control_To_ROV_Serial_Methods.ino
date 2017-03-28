void MoveCameraToSetPosition(int pan_new, int tilt_new)
{
static int pan_prev = -999;
static int tilt_prev = -999;
bool localDebug = false;

    if( localDebug) LogMsg("Begin MoveCameraToSetPosition(); pan_new = " + String(pan_new) + ", pan_prev = " + String(pan_prev) + ", tilt_new = " + String(tilt_new) + ", tilt_prev = " + String(tilt_prev));

    // The '1' provides a buffer against slight changes in readings. (Sometimes the pot value read will bounce.  Ex. 72 to 73)
    if((pan_new < (pan_prev - 2)) || (pan_new > (pan_prev + 2)) || (tilt_new < (tilt_prev - 2)) || (tilt_new > (tilt_prev + 2)))
    {
        ClearCommandToROVDataStructure();
    
        CommandToROV.command            = CMD_MOVECAMERA;
        CommandToROV.cameraMoveMethod   = CMD_CAMERAMOVEMETHOD_SET_POSITION;
        CommandToROV.cameraPanPos       = pan_new;
        CommandToROV.cameraTiltPos      = tilt_new;
        
        //SendCommandToROVAndWaitForReadyResponse();            
        SendCommandToROV(false);
        
        pan_prev = pan_new;
        tilt_prev = tilt_new;
    }
    else
    {
        if( localDebug) LogMsg(F("-- New value not worth sending to ROV"));
    }

    if( localDebug) LogMsg(F("End MoveCameraToSetPosition()"));
}

void MoveCameraWithDirectionAndImcrement(int _pandirection, int _panincrement, int _tiltdirection, int _tiltincrement)
{
bool localDebug = true;

    if( localDebug) LogMsg("Begin MoveCameraWithDirectionAndImcrement(); _pandirection = " + String(_pandirection) + ", _panincrement = " + String(_panincrement) + ", _tiltdirection = " + String(_tiltdirection) + ", _tiltincrement = " + String(_tiltincrement));

    ClearCommandToROVDataStructure();
    
    CommandToROV.command                = CMD_MOVECAMERA;
    CommandToROV.cameraMoveMethod       = CMD_CAMERAMOVEMETHOD_DIRECTION_INCREMENT;
    CommandToROV.cameraPanDirection     = _pandirection;
    CommandToROV.cameraPanIncrement     = _panincrement;
    CommandToROV.cameraTiltDirection    = _tiltdirection;
    CommandToROV.cameraTiltIncrement    = _tiltincrement;
        
    SendCommandToROV(false);
        
    if( localDebug) LogMsg(F("End MoveCameraWithDirectionAndImcrement()"));
}

void GetROVData()
{
bool localDebug = false;

    if( localDebug) LogMsg(F("Begin GetROVData()"));

    ClearCommandToROVDataStructure();
    CommandToROV.command = CMD_GETROVDATA;
        
    //SendCommandToROVAndWaitForReadyResponse();            
    SendCommandToROV(false);

    if( localDebug) LogMsg(F("End GetROVData()"));
}

void SendCommandToROVAndWaitForReadyResponse()
{
    SendCommandToROV(true);
}

void SendCommandToROV(bool waitForReadyResponse)
{
static int msgCount = 1;

    LogMsg(F("Begin SendCommandToROV()"));

    CommandToROV.msgCount = msgCount;
    LogMsg(CommandToROVToString());

    ET_SendComandToROV.sendData();
    msgCount++;

    if(waitForReadyResponse)
        GetResponseFromROV();
    
    LogMsg(F("End SendCommandToROV()"));
}

void GetResponseFromROV()
{
const int DELAY_TIME_BETWEEN_ATTEMPTED_READS = 400;
const int NUMBER_OF_TIMES_TO_ATTEMPT_READ_BEFORE_RETURNING = 10;

    LogMsg(F("Begin GetResponseFromROV()"));

    int numberOfTries = NUMBER_OF_TIMES_TO_ATTEMPT_READ_BEFORE_RETURNING;
    while(numberOfTries > 0)
    {
        if(ET_GetResponseFromROV.receiveData())
        {
            LogMsg(F("-- Got ROV response:"));
            LogMsg(ROVResponseToString());
            
            parseROVResponse();
            break;
        }
                
        numberOfTries--;
        delay(DELAY_TIME_BETWEEN_ATTEMPTED_READS);
    }
        
    if(numberOfTries < 1)
        LogMsg(F("-- Never got Response from ROV"));

    LogMsg(F("End GetResponseFromROV()"));
}

void parseROVResponse()
{
    LogMsg(F("Begin parseROVResponse()"));

    currentROVData.cameraCurrentPanPos = ResponseFromROV.cameraCurrentPanPos;
    currentROVData.cameraCurrentTiltPos = ResponseFromROV.cameraCurrentTiltPos;

    LogMsg(F("End parseROVResponse():"));
    LogMsg(CurrentROVDataToString());    
}

String ROVResponseToString()
{
String sret = "";
String linePrefix = "  - ";
String linePostfix = "\r\n";

    sret += linePrefix + "msgCount: " + String(ResponseFromROV.msgCount) + linePostfix;
    sret += linePrefix + "response: " + String(ResponseFromROV.response) + linePostfix;
    sret += linePrefix + "cameraCurrentPanPos: " + String(ResponseFromROV.cameraCurrentPanPos) + linePostfix;
    sret += linePrefix + "cameraCurrentTiltPos: " + String(ResponseFromROV.cameraCurrentTiltPos) + linePostfix;
    sret += linePrefix + "note: '" + String(ResponseFromROV.note) + "'" + linePostfix;
    
    return sret;
}

void ClearCommandToROVDataStructure()
{
    CommandToROV.msgCount               = -1;
    CommandToROV.command                = CMD_NO_COMMAND;
    CommandToROV.cameraMoveMethod       = CMD_CAMERAMOVEMETHOD_SET_POSITION;
    CommandToROV.cameraPanPos           = CMD_CAMERA_SET_POSTION_DEFAULT;
    CommandToROV.cameraTiltPos          = CMD_CAMERA_SET_POSTION_DEFAULT;
    CommandToROV.cameraPanDirection     = CMD_CAMERA_DIRECTION_PAN_LEFT;
    CommandToROV.cameraPanIncrement     = CMD_CAMERA_DIRECTION_INCREMENT_DEFAULT;
    CommandToROV.cameraTiltDirection    = CMD_CAMERA_DIRECTION_TILT_UP;
    CommandToROV.cameraTiltIncrement    = CMD_CAMERA_DIRECTION_INCREMENT_DEFAULT;
}

String CommandToROVToString()
{
String sret = "";
String linePrefix = "  - ";
String linePostfix = "\r\n";

    sret += "CommandToROV" + linePostfix;
    sret += linePrefix + "msgCount: " + String(CommandToROV.msgCount) + linePostfix;
    sret += linePrefix + "command: " + String(CommandToROV.command) + linePostfix;
    sret += linePrefix + "cameraMoveMethod: " + String(CommandToROV.cameraMoveMethod) + linePostfix;
    sret += linePrefix + "cameraPanPos: " + String(CommandToROV.cameraPanPos) + linePostfix;
    sret += linePrefix + "cameraTiltPos: " + String(CommandToROV.cameraTiltPos) + linePostfix;
    sret += linePrefix + "cameraPanDirection: " + String(CommandToROV.cameraPanDirection) + linePostfix;
    sret += linePrefix + "cameraPanIncrement: " + String(CommandToROV.cameraPanIncrement) + linePostfix;
    sret += linePrefix + "cameraTiltDirection: " + String(CommandToROV.cameraTiltDirection) + linePostfix;
    sret += linePrefix + "cameraTiltIncrement: " + String(CommandToROV.cameraTiltIncrement) + linePostfix;
    
    return sret;
}

String CurrentROVDataToString()
{
String sret = "";
String linePrefix = "  - ";
String linePostfix = "\r\n";

    sret += "currentROVData" + linePostfix;
    sret += linePrefix + "cameraCurrentPanPos: " + String(currentROVData.cameraCurrentPanPos) + linePostfix;
    sret += linePrefix + "cameraCurrentTiltPos: " + String(currentROVData.cameraCurrentTiltPos) + linePostfix;
    
    return sret;
}

