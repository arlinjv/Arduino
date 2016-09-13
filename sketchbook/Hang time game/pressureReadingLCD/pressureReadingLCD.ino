#include <SoftwareSerial.h>

// Serial output code borrowed from http://www.arduino.cc/en/Tutorial/Graph

int txPin = 2;
SoftwareSerial LCD = SoftwareSerial(0, txPin); //wanted this to be in LCD tab, but compiler stated LCD below was out of scope.

int currentReading;
int lowestReading = 1023;
String readingStr = "";
String minStr = "";

void setup() {
  // initialize the serial communication:
  Serial.begin(115200);
  
  setupLCD();
  
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
  selectLineOne();
  LCD.print(readingStr);
  Serial.println(readingStr);

  minStr="min:     ";
  if (lowestReading < 1000) {
    minStr+=" ";
  }
  minStr += lowestReading;
    selectLineTwo();
  LCD.print(minStr);

  // wait a bit for the analog-to-digital converter
  // to stabilize after the last reading:
  delay(100);
}
