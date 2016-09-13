// sketch used to implement new LCD class to replace old approach

#include <SoftwareSerial.h>
#include "SerLCD_Class.h"
// Serial output code borrowed from http://www.arduino.cc/en/Tutorial/Graph

int lcdPin = 2;

int currentReading;
int lowestReading = 1023;
String readingStr = "";
String minStr = "";
String lineOne = "Line 1 - String";
//SoftwareSerial LCD = SoftwareSerial(0, pin); 

SerLCD lcd(lcdPin);

void setup() {
  // initialize the serial communication:
  Serial.begin(115200);
  
  lcd.setupLCD();
  
}

void loop() {
  currentReading = analogRead(A0);
  
  if (currentReading < lowestReading) {
    lowestReading = currentReading;
  }

  readingStr="Current: ";
  if (currentReading < 1000) {
    readingStr+=" ";
  }
  readingStr+= currentReading;
  lcd.writeLineOne(readingStr);
  Serial.println(readingStr);

  minStr="min:     ";
  if (lowestReading < 1000) {
    minStr+=" ";
  }
  minStr += lowestReading;
  lcd.writeLineTwo(minStr);

  // wait a bit for the analog-to-digital converter
  // to stabilize after the last reading:
  delay(100);
}

