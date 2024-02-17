#include "Arduino.h"
#include <SoftwareSerial.h>

const byte rxPin = 4;
const byte txPin = 2;
SoftwareSerial BTSerial(rxPin, txPin); // RX TX

void setup() {
    // define pin modes for tx, rx:
    pinMode(rxPin, INPUT);
    pinMode(txPin, OUTPUT);
    BTSerial.begin(9600);
    Serial.begin(9600);
}

String messageBuffer = "";
String message = "";
char dataByte;

void loop() 
{
    while (BTSerial.available()) 
    {
        dataByte = BTSerial.read();
        Serial.write(dataByte);
    }
        while(Serial.available()) 
        {
            dataByte = Serial.read();
            BTSerial.write(dataByte);
        }
    
}