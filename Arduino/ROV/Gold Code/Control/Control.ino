//---------
// Control
//---------
#include <EasyTransfer.h>

#define CMD_NO_COMMAND 0
#define CMD_MOVECAMERA 1
#define CMD_GETROVDATA 2
#define CMD_CAMERAMOVEMETHOD_SET_POSITION true
#define CMD_CAMERAMOVEMETHOD_DIRECTION_INCREMENT false
#define CMD_CAMERA_SET_POSTION_DEFAULT      30
#define CMD_CAMERA_DIRECTION_PAN_DONT_MOVE  0
#define CMD_CAMERA_DIRECTION_PAN_LEFT       1
#define CMD_CAMERA_DIRECTION_PAN_RIGHT      2
#define CMD_CAMERA_DIRECTION_TILT_DONT_MOVE 0
#define CMD_CAMERA_DIRECTION_TILT_DOWN      1
#define CMD_CAMERA_DIRECTION_TILT_UP        2
#define CMD_CAMERA_DIRECTION_INCREMENT_DONT_MOVE    0
#define CMD_CAMERA_DIRECTION_INCREMENT_DEFAULT      5
#define CMD_CAMERA_DIRECTION_INCREMENT              10
#define RSP_NO_RESPONSE 0
#define ROV_RESPONSE_READY 1

typedef struct {
    int cameraCurrentPanPos;     
    int cameraCurrentTiltPos;    
} ROV_DATA;
ROV_DATA currentROVData;

// Blinking Status LED
#define BLINKPIN  13

// Joystick
#define CAMERA_POT_PAN          A0
#define CAMERA_POT_TILT         A1
#define CAMERA_JOYSTICK_PAN     A2
#define CAMERA_JOYSTICK_TILT    A3
#define CAMERA_JOYSTICK_BUTTON  47
#define CAMERA_JOYSTICK_PAN_CENTER          89
#define CAMERA_JOYSTICK_PAN_CENTER_SPAN     10
#define CAMERA_JOYSTICK_TILT_CENTER         90
#define CAMERA_JOYSTICK_TILT_CENTER_SPAN    10


EasyTransfer ET_SendComandToROV, ET_GetResponseFromROV; 
#define ET_NOTE_LEN 63
struct SEND_DATA_STRUCTURE {        // Must match the RECEIVE_DATA_STRUCTURE in the ROV code
    int msgCount;
    int command;
    bool cameraMoveMethod;          // TRUE = set position; FALSE = Direction w/ increment
    int cameraPanPos;               // Camera pan position to move to
    int cameraTiltPos;              // Camera tilt position to move to
    int cameraPanDirection;         // Camera pan direction to move
    int cameraPanIncrement;         // Camera pan increment to move
    int cameraTiltDirection;        // Camera tilt direction to move
    int cameraTiltIncrement;        // Camera tilt increment to move
};
struct RECEIVE_DATA_STRUCTURE {     // Must match the SEND_DATA_STRUCTURE in the ROV code
    int msgCount;
    int response;
    int cameraCurrentPanPos;        // Current camera pan position
    int cameraCurrentTiltPos;       // Current camera tilt position
    char note[ET_NOTE_LEN + 1];
};
SEND_DATA_STRUCTURE CommandToROV;
RECEIVE_DATA_STRUCTURE ResponseFromROV;

void setup()
{
    Serial.begin(9600);                      
    LogMsg("In-Deep ROV project Mark I.");
    LogMsg("Control Panel Module v1.0");
    pinMode(BLINKPIN, OUTPUT);               

    pinMode(CAMERA_JOYSTICK_BUTTON, INPUT);
    
    Serial1.begin(9600);
    ET_SendComandToROV.begin(details(CommandToROV), &Serial1);  
    ET_GetResponseFromROV.begin(details(ResponseFromROV), &Serial1);
}
//======================================================
void loop()
{
bool refreshDisplay = true;
static unsigned long nextTime_GetROVData = 0;       // The next time to get ROV Data
const unsigned long ROVDATA_INTERVAL = 1000 * 10;   // Get ROV Data every 10 seconds
int camerapan;
int cameratilt;
int camerabutton;

    BlinkActivityLED();

    // Pots
    camerapan = ReadCameraPotPan();
    cameratilt = ReadCameraPotTilt();
    MoveCameraToSetPosition(camerapan, cameratilt);

    // Joystick
    camerapan = ReadCameraJoystickPan();
    cameratilt = ReadCameraJoystickTilt();
    camerabutton = ReadCameraJoystickButton();    
    LogMsg("pan = " + String(camerapan) + ", tilt = " + String(cameratilt) + ", button = " + String(camerabutton));
    bool needToMoveCamera = false;
    int pandirection = CMD_CAMERA_DIRECTION_PAN_DONT_MOVE;
    int panincrement = CMD_CAMERA_DIRECTION_INCREMENT_DONT_MOVE;
    int tiltdirection = CMD_CAMERA_DIRECTION_TILT_DONT_MOVE;
    int tiltincrement = CMD_CAMERA_DIRECTION_INCREMENT_DONT_MOVE;
    if( camerapan < (CAMERA_JOYSTICK_PAN_CENTER - CAMERA_JOYSTICK_PAN_CENTER_SPAN))
    {
        pandirection = CMD_CAMERA_DIRECTION_PAN_LEFT;
        panincrement = CMD_CAMERA_DIRECTION_INCREMENT;
        needToMoveCamera = true;
    }
    if( camerapan > (CAMERA_JOYSTICK_PAN_CENTER + CAMERA_JOYSTICK_PAN_CENTER_SPAN))
    {
        pandirection = CMD_CAMERA_DIRECTION_PAN_RIGHT;
        panincrement = CMD_CAMERA_DIRECTION_INCREMENT;
        needToMoveCamera = true;
    }
    if( cameratilt < (CAMERA_JOYSTICK_TILT_CENTER - CAMERA_JOYSTICK_TILT_CENTER_SPAN))
    {
        tiltdirection = CMD_CAMERA_DIRECTION_TILT_DOWN;
        tiltincrement = CMD_CAMERA_DIRECTION_INCREMENT;
        needToMoveCamera = true;
    }
    if( cameratilt > (CAMERA_JOYSTICK_TILT_CENTER + CAMERA_JOYSTICK_TILT_CENTER_SPAN))
    {
        tiltdirection = CMD_CAMERA_DIRECTION_TILT_UP;
        tiltincrement = CMD_CAMERA_DIRECTION_INCREMENT;
        needToMoveCamera = true;
    }
    if(needToMoveCamera)
        MoveCameraWithDirectionAndImcrement(pandirection, panincrement, tiltdirection, tiltincrement);



    // See if it is time to update the ROVData
    if( millis() > nextTime_GetROVData)
    {
        GetROVData();
        nextTime_GetROVData = millis() + ROVDATA_INTERVAL;

//        refreshDisplay = true;     do this only when we get the message!!!!
    }

    // Update the display
    if( refreshDisplay)
    {
        refreshDisplay = false;    
    }
    
    // you should make this delay shorter then your transmit delay or else messages could be lost??????????????
    delay(250);
}

//======================================================

// Blinks the small LED on the board.  Used to indicate activity.
void BlinkActivityLED()
{
static bool blinkOnOff = true;

    digitalWrite(BLINKPIN, blinkOnOff); // We typically write LOW or HIGH to the pin but, since, those
                                        //  value map to 1 and 0 we can also write the bool value of blinkOnOff.    
    blinkOnOff = !blinkOnOff;           
}


