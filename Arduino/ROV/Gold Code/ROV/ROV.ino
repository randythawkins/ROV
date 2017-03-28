//-----
// ROV
//-----
#include <EasyTransfer.h>
#include <Servo.h>

#define CMD_NO_COMMAND 0
#define CMD_MOVECAMERA 1
#define CMD_GETROVDATA 2
#define CMD_CAMERAMOVEMETHOD_SET_POSITION true
#define CMD_CAMERAMOVEMETHOD_DIRECTION_INCREMENT false
#define CMD_CAMERA_SET_POSTION_DEFAULT 30
#define CMD_CAMERA_DIRECTION_PAN_DONT_MOVE  0
#define CMD_CAMERA_DIRECTION_PAN_LEFT       1
#define CMD_CAMERA_DIRECTION_PAN_RIGHT      2
#define CMD_CAMERA_DIRECTION_TILT_DONT_MOVE 0
#define CMD_CAMERA_DIRECTION_TILT_DOWN      1
#define CMD_CAMERA_DIRECTION_TILT_UP        2
#define RSP_NO_RESPONSE 0
#define RSP_ROV_READY 1

typedef struct {
    int panPos;     
    int tiltPos;    
} CAMERA_POSITION_DATA;
CAMERA_POSITION_DATA cameraPositionData;

EasyTransfer ET_SendResponseToComandToControl, ET_GetCommandFromControl; 
#define ET_NOTE_LEN 63
struct SEND_DATA_STRUCTURE {        // Must match the RECEIVE_DATA_STRUCTURE in the Control code
    int msgCount;
    int response;
    int cameraCurrentPanPos;        // Current camera pan position
    int cameraCurrentTiltPos;       // Current camera tilt position
    char note[ET_NOTE_LEN + 1];
};
struct RECEIVE_DATA_STRUCTURE {     // Must match the SEND_DATA_STRUCTURE in the Control code
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
SEND_DATA_STRUCTURE ResponseToControl;
RECEIVE_DATA_STRUCTURE CommandFromControl;

Servo PanServo;
Servo TiltServo;

// Blinking Status LED
#define BLINKPIN  13

// Servos and Pots mapping values
#define CAMERA_PAN_LOWER_LIMIT 0
#define CAMERA_PAN_UPPER_LIMIT 180
#define CAMERA_TILT_LOWER_LIMIT 0
#define CAMERA_TILT_UPPER_LIMIT 150
#define CAMERA_PAN_CENTER_POSITION 85
#define CAMERA_TILT_CENTER_POSITION 102
//#define MAP_SERVO_POT_LOWER_LIMIT 1
//#define MAP_SERVO_POT_UPPER_LIMIT 1000
//#define MAP_SERVO_LOWER_LIMIT 1                 // todo - need to determine actual value
//#define MAP_SERVO_UPPER_LIMIT 180               // todo - need to determine actual value

void setup()
{
    Serial.begin(9600);
    LogMsg(F("In-Deep ROV project Mark I."));
    LogMsg(F("ROV Module v1.0"));
    pinMode(BLINKPIN, OUTPUT);

    // Servos
    PanServo.attach(3); 
    TiltServo.attach(2);

    Serial1.begin(9600);
    ET_SendResponseToComandToControl.begin(details(ResponseToControl), &Serial1);  
    ET_GetCommandFromControl.begin(details(CommandFromControl), &Serial1);

    CycleCamera(); 
}
//======================================================
void loop()
{
    BlinkActivityLED();

    if( GetCommandFromControl())
    {
        CheckAndMoveCamera();
        
//        SendReadyResponseToControl();
    }
    
    // you should make this delay shorter then your transmit delay or else messages could be lost
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

