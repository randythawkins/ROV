String GetROVData()
{
  /*
    LogMsg( "Begin GetROVData()");
    String ret = ROV_RESPONSE_SUCCESS;
    
    String msg = CMD_GETROVDATA + String("") + EOM;
    SendCommandToROV(msg);
    
    String response = GetResponseFromROV();
    if( response == ROV_RESPONSE_ERROR)
    {
    }
    else
    {
        // parse the string and place the values in the global struct
    }

    LogMsg( "End GetROVData(); ret = '" + ret + "'");
    return ret;
    */
    return "";
}

// Will check the serial port from the Control to see if a command has been sent.
//  It will try for a period of time and then return if nothing is found.  The 
//  return is so that the rest of the code has the ability to do something if
//  it needs to (kinda like the Windows WM_IDLE message).
CONTROL_COMMAND GetCommandFromControl()
{
const int MAX_MESSAGE_LENGTH = 20;    
boolean done = false;
boolean stillReading = true;
const int DELAY_TIME_BETWEEN_ATTEMPTED_READS = 10;    // In milliseconds
const int NUMBER_OF_TIMES_TO_ATTEMPT_READ_BEFORE_RETURNING = 5;
int attemptReadCounter = 0;
char charRead = 0;
String commandMessage = "";
boolean localDebug = true;

    LogMsg( "Begin GetCommandFromControl()");
    
    CONTROL_COMMAND controlCommand = getNewControlCommand();
    controlCommand.command = MSG_FROM_CONTROL_NOOP;     // Default to noop

    if(localDebug) LogMsg("Beginning 'while(!done)' loop");
    while(!done)
    {
        if(localDebug) LogMsg("Beginning 'if(ControlSerial.available() > 0)' loop");
        if(ControlSerial.available() > 0)
        {
            charRead = ControlSerial.read();

            if(localDebug) LogMsg("--> First char read - '" + String(charRead) + "'");
            
            if( charRead == MSG_COUNTER_BEGIN[0])
            {   // We have a valid marker for the beginning of a Command; Get the rest of it.
                commandMessage.concat(charRead);
                while(stillReading)
                {
                    charRead = ControlSerial.read();
                    if(localDebug) LogMsg("--> Next char read - '" + String(charRead) + "'");
                    commandMessage.concat(charRead);
                    if(localDebug) LogMsg("commandMessage at this point - '" + commandMessage + "'");
                    
                    if(commandMessage.length() > MAX_MESSAGE_LENGTH)
                    {
                        String s = "* commandMessage exceeded a length of " + String(MAX_MESSAGE_LENGTH) + "; ABORTING!";
                        if(localDebug) LogMsg(s);
                        
                        controlCommand.errorCode = CONTROL_COMMAND_ERROR_ERROR_RESPONSE_MSG_TOO_LONG;
                        controlCommand.errorMsg = s;
                        
                        stillReading = false;
                    }
                    
                    if(charRead == EOM[0])
                    {
                        if(localDebug) LogMsg("Inside 'if(charRead == EOM[0])', setting stillReading to false");
                        stillReading = false;
                    }
                }

                controlCommand.commandStringFromControl = commandMessage;
                controlCommand = parseControlCommand(controlCommand);
                
                if(localDebug) LogMsg("Setting 'done = true'");
                done = true;
            }
        }
        else
        {
            if(localDebug) LogMsg("Inside 'if(ControlSerial.available() > 0)...else' statement; doing delay()");
            delay(DELAY_TIME_BETWEEN_ATTEMPTED_READS);
            attemptReadCounter++;
            if(localDebug) LogMsg("Incremented attemptReadCounter to " + String(attemptReadCounter));
            if(attemptReadCounter == DELAY_TIME_BETWEEN_ATTEMPTED_READS)
            {
                if(localDebug) LogMsg("Inside 'if(attemptReadCounter == DELAY_TIME_BETWEEN_ATTEMPTED_READS); Setting 'done = true'");
                done = true;
            }
        }
        if(localDebug) LogMsg("Ended 'if(ControlSerial.available() > 0)' loop");
    }
    if(localDebug) LogMsg("Ended 'while(!done)' loop");
        
    LogMsg("End GetCommandFromControl()");
    LogMsg("  - controlCommand:");
    LogMsg(ControlCommandToString(controlCommand));
    return controlCommand;
}

String ControlCommandToString(CONTROL_COMMAND controlCommand)
{
String sret = "";
String linePrefix = "  - ";

    sret += linePrefix + "commandStringFromControl: " + controlCommand.commandStringFromControl;
    sret += linePrefix + "messageCounter: " + String(controlCommand.messageCounter);
    sret += linePrefix + "command: " + controlCommand.command;
    sret += linePrefix + "camera_position.pan: " + String(controlCommand.camera_position.pan);
    sret += linePrefix + "camera_position.tilt: " + String(controlCommand.camera_position.tilt);
    sret += linePrefix + "errorCode: " + String(controlCommand.errorCode);
    sret += linePrefix + "errorMsg: " + controlCommand.errorMsg;

    return sret;
}

void SendReadyToControl()
{
    LogMsg("Begin SendReadyToControl()");

    String msg = ROV_RESPONSE_READY + String(CMD_END) + String("") + EOM;
    SendResponseToControl(msg);
    
    LogMsg("End SendReadyToControl()");
}


void SendResponseToControl( String msg)
{
static int msgCount = 1;

    LogMsg( "Begin SendResponseToControl(); msg = '" + msg + "'");
    
    String msgWithCounter = "[" + String(msgCount) + "]" + msg;
    LogMsg( "msgWithCounter = '" + msgWithCounter + "'");
    ControlSerial.print( msgWithCounter);
    msgCount++;
    
    LogMsg( "End SendResponseToControl()");
}

// This method will take the commandStringFromControl value from the passed-in
//  controlCommand structure, break it out into its individual parts, place those
//  parts back into the controlCommand structure, and return that structure back
//  to the calling code.
CONTROL_COMMAND parseControlCommand(CONTROL_COMMAND controlCommand)
{
int startIndex;
int endIndex;
String s;

    LogMsg("Begin parseControlCommand()");

    String workingCommandString = controlCommand.commandStringFromControl;

    // Get the message counter
    //  Starts with "[23]C1_142|26!"; Obtains "23" as the messageCounter; Continues with "C1_142|26!" for the next step
    startIndex = 0;
    endIndex = workingCommandString.indexOf(MSG_COUNTER_END);
    controlCommand.messageCounter = (workingCommandString.substring(startIndex + 1, endIndex)).toInt();
    workingCommandString = workingCommandString.substring(endIndex);

    // Get the Command
    //  Starts with "C1_142|26!"; Obtains "C1" as the controlCommand; Continues with "142|26!" for the next step
    startIndex = 0;
    endIndex = workingCommandString.indexOf(CMD_END);
    controlCommand.command = workingCommandString.substring(startIndex + 1, endIndex);
    workingCommandString = workingCommandString.substring(endIndex + 1);

    if( controlCommand.command == CMD_MOVECAMERA)
    {   // Format example: "142|26!"
        startIndex = 0;
        endIndex = workingCommandString.indexOf(EOM);
        s = workingCommandString.substring(startIndex, endIndex);   // Should have "142|26" as a result
        endIndex = s.indexOf(DATA_SEPARATOR);
        if( endIndex != -1)
        {
            controlCommand.camera_position.pan = (s.substring(startIndex, endIndex)).toInt();
            
            if( endIndex < ( s.length() - 1))
                controlCommand.camera_position.tilt = (s.substring(endIndex + 1)).toInt();
            else
            {
                s = "Camera position did not contain a tilt value";
        
                LogMsg("*** " + s);
                controlCommand.errorCode = CONTROL_COMMAND_ERROR_NO_TILT_VALUE_PROVIDED;
                controlCommand.errorMsg = s;
            }
        }
        else
        {
            s = "Camera position did not contain a data separator ('|')";
    
            LogMsg("*** " + s);
            controlCommand.errorCode = CONTROL_COMMAND_ERROR_NO_DATA_SEPARATOR_PROVIDED;
            controlCommand.errorMsg = s;
        }
    }
    else if( controlCommand.command == CMD_GETROVDATA)
    {
        
    }
    else
    {   // Error...this should not happen
        s = "parseControlCommand(): Unable to parse the controlCommand '" + controlCommand.command + "'";
        
        LogMsg("*** " + s);

        controlCommand.errorCode = CONTROL_COMMAND_ERROR_UNKNOWN_COMMAND_VALUE;
        controlCommand.errorMsg = s;
    }

    LogMsg("End parseControlCommand()");
    return controlCommand;
}

// Creates a new ControlCommand structure and sets default values for all members.
CONTROL_COMMAND getNewControlCommand()
{
    CONTROL_COMMAND controlCommand;

    controlCommand.commandStringFromControl = "";
    controlCommand.messageCounter = 0;
    controlCommand.command = "";
    controlCommand.camera_position.pan = CAMERA_PAN_CENTER_POSITION;
    controlCommand.camera_position.tilt = CAMERA_TILT_CENTER_POSITION;
    controlCommand.errorCode = CONTROL_COMMAND_ERROR_NO_ERROR;
    controlCommand.errorMsg = "";
    
    return controlCommand;    
}






