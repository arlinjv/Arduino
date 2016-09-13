// sketch used to implement new LCD class to replace old approach

#include <SoftwareSerial.h>
#include "SerLCD_Class.h"
// Serial output code borrowed from http://www.arduino.cc/en/Tutorial/Graph

int lcdPin = 2;

int currentReading;
int lowestReading = 1023;
String readingStr = "";
String minStr = "";

//SoftwareSerial LCD = SoftwareSerial(0, pin); 

SerLCD lcd(lcdPin);

void setup() {
  // initialize the serial communication:
  Serial.begin(115200);
  
  lcd.setupLCD();
  
  lcd.writeLineOne();
  
  lcd.writeLineTwo();
}

void loop(){
  // do nothing for now
}

/* implement later:
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
*/
