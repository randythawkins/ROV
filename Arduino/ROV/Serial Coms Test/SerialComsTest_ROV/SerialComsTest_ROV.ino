#include <Servo.h>


Servo PanServo;
Servo TiltServo;


void setup() {
    Serial.begin(9600);
    Serial1.begin(57600);

    PanServo.attach(3); 
//    TiltServo.attach(2);

    Serial.println("ROV running");
}

void loop() {

int byteReceived = 0;
String s;
int count = 0;

unsigned long begintime, totaltime;

    count = Serial1.available();
    if(count > 0)
    {
        Serial.println("1 count = " + String(count));
    
        s = Serial1.readString();// read the incoming data as string
        Serial.println("1 echo: " + s);

        PanServo.write(s.toInt()); 
        delay(10);
        //TiltServo.write(s.toInt()); 
        //delay(10);
        
    }

    count = Serial.available();
    if(count > 0)
    {
        s = Serial.readString();// read the incoming data as string
        begintime = millis();
        //Serial.println("2 count = " + String(count));
    
//        s = Serial.readString();// read the incoming data as string
        //Serial.println("2 echo: " + s);

        PanServo.write(s.toInt());
        totaltime = millis() - begintime;
        Serial.println( "time = " + String(totaltime)); 
        delay(10);
        //TiltServo.write(s.toInt()); 
        //delay(10);
        
    }
}
