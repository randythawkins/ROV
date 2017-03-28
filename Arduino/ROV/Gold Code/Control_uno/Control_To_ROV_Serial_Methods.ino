/*
    1. set camera servos position.
    2. get data
        2a. camera servos positions
        2b. water temp, distance from bottom, etc
    
    [{message count}][{command}]_[{data}][{end of command}]

    [[{message count}]]
        Continuous incrementing counter used to identify a command across both Control and ROV
  
    [{command}]
        [C1] Command #1 - Set Camera Position
            [{data}]
                [{x|y}] - X and Y values for camera position
        [C2] Command #2 - Get ROV data
            [{data}] - N/A
    
    [{end of command}]
        [!] - Indicates the end of the communication
    
    Examples:
        [23]C1_142|26!  - Move camera to x position of 142 and y position 26
        [106]C2_!       - Get ROV data
*/

void MoveCamera(int pan_new, int tilt_new)
{
static int pan_prev = -999;
static int tilt_prev = -999;

    LogMsg("Begin MoveCamera(); pan_new = " + String(pan_new) + ", pan_prev = " + String(pan_prev) + ", tilt_new = " + String(tilt_new) + ", tilt_prev = " + String(tilt_prev));

    // The '1' provides a buffer against slight changes in readings. (Sometimes the pot value read will bounce.  Ex. 72 to 73)
    if((pan_new < (pan_prev - 1)) || (pan_new > (pan_prev + 1)) || (tilt_new < (tilt_prev - 1)) || (tilt_new > (tilt_prev + 1)))
    {
        String msg = CMD_MOVECAMERA + String(CMD_END) + String(pan_new) + DATA_SEPARATOR + String(tilt_new) + EOM;
        SendCommandToROVAndWaitForReadyResponse(msg);
        
        pan_prev = pan_new;
        tilt_prev = tilt_new;
    }

    LogMsg("End MoveCamera()");
}

String GetROVData()
{
    /*
    LogMsg("Begin GetROVData()");
    String ret = ROV_RESPONSE_SUCCESS;
    
    String msg = CMD_GETROVDATA + String(CMD_END) + String("") + EOM;
    SendCommandToROV(msg, false);
    
    ROV_RESPONSE response = GetResponseFromROV();
    if( response.errorCode == ROV_RESPONSE_ERROR)
    {
    }
    else
    {
        // parse the string and place the values in the global struct
    }

    LogMsg("End GetROVData(); ret = '" + ret + "'");
*/    
    return "";
    
}

ROV_RESPONSE SendCommandToROVAndWaitForReadyResponse(String msg)
{
    return SendCommandToROV(msg, true);
}

ROV_RESPONSE SendCommandToROV(String msg, bool waitForReadyResponse)
{
static int msgCount = 1;
ROV_RESPONSE rov_response = getNewROVResponse();

    LogMsg("Begin SendCommandToROV(); msg = '" + msg + "'");
    
    String msgWithCounter = "[" + String(msgCount) + "]" + msg;
    LogMsg("msgWithCounter = '" + msgWithCounter + "'");
    ROVSerial.print( msgWithCounter);
    msgCount++;

    if(waitForReadyResponse)
    {
        int numberOfTries = 5;
        while(numberOfTries > 0)
        {
            rov_response = GetResponseFromROV();
            if(rov_response.response == ROV_RESPONSE_READY)
                break;
                
            numberOfTries--;
            delay(10);
        }
        
        if(numberOfTries < 1)
            LogMsg( "* Never got Ready response from ROV");
    }
    
    LogMsg("End SendCommandToROV()");
    return rov_response;
}

String ROVResponseToString(ROV_RESPONSE rovResponse)
{
String sret = "";
String linePrefix = "  - ";

    sret += linePrefix + "responseStringFromROV: " + rovResponse.responseStringFromROV;
    sret += linePrefix + "messageCounter: " + String(rovResponse.messageCounter);
    sret += linePrefix + "response: " + rovResponse.response;
    sret += linePrefix + "errorCode: " + String(rovResponse.errorCode);
    sret += linePrefix + "errorMsg: " + rovResponse.errorMsg;
    
    return sret;
}

// Creates a new ROVResponse structure and sets default values for all members.
ROV_RESPONSE getNewROVResponse()
{
    ROV_RESPONSE rovResponse;

    rovResponse.responseStringFromROV = "";
    rovResponse.messageCounter = 0;
    rovResponse.response = "";
    rovResponse.errorCode = ROV_RESPONSE_ERROR_NO_ERROR;
    rovResponse.errorMsg = "";
    
    return rovResponse;    
}

ROV_RESPONSE GetResponseFromROV()
{
const int MAX_MESSAGE_LENGTH = 20;    
boolean done = false;
boolean stillReading = true;
const int DELAY_TIME_BETWEEN_ATTEMPTED_READS = 10;    // In milliseconds
const int NUMBER_OF_TIMES_TO_ATTEMPT_READ_BEFORE_RETURNING = 5;
int attemptReadCounter = 0;
char charRead = 0;
String rovMessage = "";
boolean localDebug = true;
    
    LogMsg("Begin GetResponseFromROV()");

    ROV_RESPONSE rovResponse = getNewROVResponse();
    rovResponse.response = ROV_RESPONSE_NOOP;       // Default to noop
    
    if(localDebug) LogMsg("Beginning 'while(!done)' loop");
    while(!done)
    {
        if(localDebug) LogMsg("Beginning 'if(ROVSerial.available() > 0)' loop");
        if(ROVSerial.available() > 0)
        {
            charRead = ROVSerial.read();

            if(localDebug) LogMsg("--> First char read - '" + String(charRead) + "'");
            
            if(charRead == MSG_COUNTER_BEGIN[0])
            {   // We have a valid marker for the beginning of a Response; Get the rest of it.
                rovMessage.concat(charRead);
                while(stillReading)
                {
                    charRead = ROVSerial.read();
                    if(localDebug) LogMsg("--> Next char read - '" + String(charRead) + "'");
                    rovMessage.concat(charRead);
                    if(localDebug) LogMsg("rovMessage at this point - '" + rovMessage + "'");

                    if(rovMessage.length() > MAX_MESSAGE_LENGTH)
                    {
                        String s = "* rovMessage exceeded a length of " + String(MAX_MESSAGE_LENGTH) + "; ABORTING!";
                        if(localDebug) LogMsg(s);
                        
                        rovResponse.errorCode = ROV_RESPONSE_ERROR_RESPONSE_MSG_TOO_LONG;
                        rovResponse.errorMsg = s;
                        
                        stillReading = false;
                    }

                    if(charRead == EOM[0])
                    {
                        if(localDebug) LogMsg("Inside 'if(charRead == EOM[0])', setting stillReading to false");
                        stillReading = false;
                    }
                }

                rovResponse.responseStringFromROV = rovMessage;
                rovResponse = parseROVResponse(rovResponse);
                
                if(localDebug) LogMsg("Setting 'done = true'");
                done = true;
            }
        }
        else
        {
            if(localDebug) LogMsg("Inside 'if(ROVSerial.available() > 0)...else' statement; doing delay()");
            delay(DELAY_TIME_BETWEEN_ATTEMPTED_READS);
            attemptReadCounter++;
            if(localDebug) LogMsg("Incremented attemptReadCounter to " + String(attemptReadCounter));
            if(attemptReadCounter == DELAY_TIME_BETWEEN_ATTEMPTED_READS)
            {
                if(localDebug) LogMsg("Inside 'if(attemptReadCounter == DELAY_TIME_BETWEEN_ATTEMPTED_READS); Setting 'done = true'");
                done = true;
            }
        }
        if(localDebug) LogMsg("Ended 'if(ROVSerial.available() > 0)' loop");
    }
    if(localDebug) LogMsg("Ended 'while(!done)' loop");
    

    LogMsg("End GetResponseFromROV()");
    LogMsg("  - rovResponse:");
    LogMsg(ROVResponseToString(rovResponse));
    
    return rovResponse;
}

// This method will take the responseStringFromROV value from the passed-in
//  rovResponse structure, break it out into its individual parts, place those
//  parts back into the rovResponse structure, and return that structure back
//  to the calling code.
ROV_RESPONSE parseROVResponse(ROV_RESPONSE rovResponse)
{
int startIndex;
int endIndex;
String s;

    LogMsg("Begin parseROVResponse()");

    String workingResponseString = rovResponse.responseStringFromROV;

    // Get the message counter
    //  Starts with "[23]R1_142|26!"; Obtains "23" as the messageCounter; Continues with "R1_142|26!" for the next step
    startIndex = 0;
    endIndex = workingResponseString.indexOf(MSG_COUNTER_END);
    rovResponse.messageCounter = (workingResponseString.substring(startIndex + 1, endIndex)).toInt();
    workingResponseString = workingResponseString.substring(endIndex);

    // Get the Response
    //  Starts with "R1_142|26!"; Obtains "R1" as the rovResponse; Continues with "142|26!" for the next step
    startIndex = 0;
    endIndex = workingResponseString.indexOf(ROV_RESPONSE_END);
    rovResponse.response = workingResponseString.substring(startIndex + 1, endIndex);
    workingResponseString = workingResponseString.substring(endIndex + 1);

    if( rovResponse.response == ROV_RESPONSE_THIS_TO_DO_NEXT)
    {   // Format example: "142|26!"
/*        
        startIndex = 0;
        endIndex = workingResponseString.indexOf(EOM);
        s = workingResponseString.substring(startIndex, endIndex);   // Should have "142|26" as a result
        endIndex = s.indexOf(DATA_SEPARATOR);
        if( endIndex != -1)
        {
            rovResponse.camera_position.pan = (s.substring(startIndex, endIndex)).toInt();
            
            if( endIndex < ( s.length() - 1))
                rovResponse.camera_position.tilt = (s.substring(endIndex + 1)).toInt();
            else
            {
                s = "Camera position did not contain a tilt value";
        
                LogMsg("*** " + s);
                rovResponse.errorCode = CONTROL_COMMAND_ERROR_NO_TILT_VALUE_PROVIDED;
                rovResponse.errorMsg = s;
            }
        }
        else
        {
            s = "Camera position did not contain a data separator ('|')";
    
            LogMsg("*** " + s);
            rovResponse.errorCode = CONTROL_COMMAND_ERROR_NO_DATA_SEPARATOR_PROVIDED;
            rovResponse.errorMsg = s;
        }
*/        
    }
    else if( rovResponse.response == ROV_RESPONSE_READY)
    {
        // Nothing to do?    <-- todo?  really?
    }
    else
    {   // Error...this should not happen
        s = "parseROVResponse(): Unable to parse the rovResponse '" + rovResponse.response + "'";
        
        LogMsg("*** " + s);

        rovResponse.errorCode = ROV_RESPONSE_ERROR_UNKNOWN_COMMAND_VALUE;
        rovResponse.errorMsg = s;
    }

    LogMsg("End parseROVResponse()");
    
    return rovResponse;
}

