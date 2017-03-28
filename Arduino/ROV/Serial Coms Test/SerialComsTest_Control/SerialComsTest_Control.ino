void setup() {
    Serial.begin(9600);
    Serial1.begin(57600);

    Serial.println("Control running");
}

void loop() {
int byteReceived = 0;
String s;

    while(Serial.available())
    {
        s = Serial.readString();// read the incoming data as string
        Serial.println("echo: " + s);
        Serial1.println(s);
    }
}
