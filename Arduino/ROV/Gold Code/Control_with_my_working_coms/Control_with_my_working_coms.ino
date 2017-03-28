//--------------
// Control File
//--------------
//#include <SoftwareSerial.h>

// SoftwareSerial communications to ROV
//#define SSerialRX 10
//#define SSerialTX 11
//SoftwareSerial ROVSerial(SSerialRX, SSerialTX);
int byteReceived;
int byteSend;
#define MSG_COUNTER_BEGIN '['
#define MSG_COUNTER_END ']'
#define CMD_MOVECAMERA "C1"
#define CMD_GETROVDATA "C2"
#define CMD_END "_"
#define DATA_SEPARATOR "|"
#define EOM "!"
#define ROV_RESPONSE_END "_"
#define ROV_RESPONSE_NOOP "1"
#define ROV_RESPONSE_READY "R1"
#define ROV_RESPONSE_THIS_TO_DO_NEXT "R2"
#define ROV_RESPONSE_ERROR_NO_ERROR                     0
#define ROV_RESPONSE_ERROR_UNKNOWN_COMMAND_VALUE        1000
#define ROV_RESPONSE_ERROR_RESPONSE_MSG_TOO_LONG        1001
typedef struct {
  int camera_position_x = -999;
  int camera_position_y = -999;
} ROV_DATA;
ROV_DATA currentROVData;
typedef struct {
    String responseStringFromROV;    // [27]C1_142|26!    <-- todo: note the correct response
    int messageCounter;
    String response;                     // R1, R2, etc
///    CAMERA_POSITION camera_position;
    int errorCode;
    String errorMsg;
} ROV_RESPONSE;

// Blinking Status LED
#define BLINKPIN  13

// Servos and Pots mapping values
#define MAP_SERVO_POT_LOWER_LIMIT 1
#define MAP_SERVO_POT_UPPER_LIMIT 1000
#define MAP_SERVO_LOWER_LIMIT 1                 // todo - need to determine actual value
#define MAP_SERVO_UPPER_LIMIT 180               // todo - need to determine actual value

void setup()
{
  
  Serial.begin(9600);                       // Start the built-in serial port
  LogMsg("In-Deep ROV project Mark I.");
  LogMsg("Control Panel Module v1.0");
  pinMode(BLINKPIN, OUTPUT);                // This is the small LED on the board.  We blink it to show activity.
  
//  ROVSerial.begin(4800);                    // Start the software serial port to communicate with the ROV 
    Serial1.begin(9600);
  
  //MoveCamera( 123, 456);
  //GetROVData();
}
//======================================================
void loop()
{
int camerapan = readCameraPanPot();
int cameratilt = readCameraTiltPot();
LogMsg("Camera Pots: Pan = " + String(camerapan) + ", Tilt = " + String(cameratilt));
MoveCamera(camerapan, cameratilt);

  if(Serial.available())
  {
    byteReceived = Serial.read();
    //ROVSerial.write(byteReceived);  // Send byte to Remote Arduino
    Serial1.write(byteReceived);  // Send byte to Remote Arduino
    BlinkActivityLED();

    delay(25);
  }

  
// del if (ROVSerial.available())  //Look for data from other Arduino
  if(Serial1.available())  //Look for data from other Arduino
   {
    BlinkActivityLED();

// del    byteSend = ROVSerial.read();    // Read received byte
    byteSend = Serial1.read();    // Read received byte
    LogMsg("sent to control from ROV - ");
    LogMsg(byteSend);        // Show on Serial Monitor
    delay(10);
        BlinkActivityLED();

   }  
  

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

/*
*Question - 
I am sending a list of servo positions via the serial connection to the arduino in the following format

1:90&2:80&3:180
Which would be parsed as:

servoId : Position & servoId : Position & servoId : Position

How would I split these values up, and convert them to an integer?

*Answer - 
This function can be used to seperate a string into pieces based on what the separating character is.

String xval = getValue(myString, ':', 0);
String yval = getValue(myString, ':', 1);

Serial.println("Y:" + yval);
Serial.print("X:" + xval);
Convert String to int

int xvalue = stringToNumber(xval);    (rth: use atoi?)
int yvalue = stringToNumber(yval);
This Chunk of code takes a string and separates it based on a given character and returns The item between the separating character

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
---------------------------------------------
*Question -
So when I send 100,50,30 arduino will translate it to

int r = 100
int g = 50
int b = 30

*Answer - 
int commaIndex = myString.indexOf(',');
//  Search for the next comma just after the first
int secondCommaIndex = myString.indexOf(',', commaIndex+1);
Then you could use that index to create a substring using the String class's substring() method. This returns a new String beginning at a particular starting index, and ending just before a second index (Or the end of a file if none is given). So you would type something akin to:

String firstValue = myString.substring(0, commaIndex);
String secondValue = myString.substring(commaIndex+1, secondCommaIndex);
String thirdValue = myString.substring(secondCommaIndex); // To the end of the string
Finally, the integer values can be retrieved using the String class's undocumented method, toInt():

int r = firstValue.toInt();
int g = secondValue.toInt();
int b = thirdValue.toInt();
*/
