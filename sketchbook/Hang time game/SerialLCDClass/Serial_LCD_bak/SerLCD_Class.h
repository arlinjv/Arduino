/*

Usage:
 - call from main script as such: SerLCD lcd(lcdPin);
 - be sure to use these includes in main script
    #include <SoftwareSerial.h> // not sure why it doesn't work by including within this file
    #include "SerLCD_Class.h"

Tested using Sparkfun serial lcd, partno LCD-00461. Appears to be discontinued.
Reference:
    http://www.sparkfun.com/datasheets/LCD/SerLCD_V2_5.PDF (pdf saved in program directory
    http://www.arduino.cc/playground/Learning/SparkFunSerLCD
    https://www.sparkfun.com/tutorials/246 (sample scripts and chart showing addresses for screen positions on 2 line 16 char display)

helpful pages:
  Using classes with Arduino: http://stackoverflow.com/questions/1735990/using-classes-with-the-arduino
  Accessing SoftwareSerial from LCD class:  
      http://arduino.cc/forum/index.php/topic,37179.0.html (SoftwareSerial causes error in library)
      http://arduino.cc/forum/index.php/topic,40435.0.html  (How to sub class serial function? )
      

*/

#ifndef SerLCD_Class_h
#define SerLCD_Class_h

#include "Arduino.h"
//#include <SoftwareSerial.h> // it seems that it will compile either way with this include here but must include in main page

class SerLCD
{
  private:
    SoftwareSerial LCD;
    int _lcdPin;
    void selectLineOne();
    void selectLineTwo();
    
  public:
    SerLCD(int pin);
    void setupLCD();
    void clearLCD();
    void writeLineOne();
    void writeLineTwo();
  
}; // don't forget the semicolon!!! 

SerLCD::SerLCD(int txPin) : LCD(SoftwareSerial(0,txPin)) // colon denotes an initialization list. still don't understand the obj() syntax
{
//  SoftwareSerial LCD = SoftwareSerial(0, pin);
  _lcdPin = txPin; //
}

void SerLCD::clearLCD()
{
   LCD.write(0xFE);   //command flag
   LCD.write(0x01);   //clear command.
}


void SerLCD::setupLCD()
{
  pinMode(_lcdPin, OUTPUT); // not sure if this is necessary
  LCD.begin(9600);
  clearLCD();
  LCD.print("LCD setup");
  delay(500);
  clearLCD();
}

void SerLCD::selectLineOne()
{  //puts the cursor at line 0 char 0.
   LCD.write(0xFE);   //command flag
   LCD.write(128);    //position
}

void SerLCD::selectLineTwo()
{  //puts the cursor at line 1 char 0.
   LCD.write(0xFE);   //command flag
   LCD.write(192);    //position
}

void SerLCD::writeLineOne()
{
  selectLineOne();
  LCD.print("Line one        ");
  
}

void SerLCD::writeLineTwo()
{
  selectLineTwo();
  LCD.print("Line two        ");
  
}

#endif
