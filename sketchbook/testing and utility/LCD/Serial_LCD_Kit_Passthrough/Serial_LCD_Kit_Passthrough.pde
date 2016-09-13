/*
Serial LCD Kit Serial Pass-through example
by: Jim Lindblom
SparkFun Electronics
date: June 16th, 2011
license: Creative Commons BY-SA 3.0

Simple code that takes serial input at 9600 baud from serial monitor
and passes that through to the serial lcd.

Connect the Serial LCD Kit as follows:
Serial LCD Kit -------------- Arduino
  Red (5V) ---------------------5V
 Black (GND) -------------------GND
 Yellow (RX) -------------------D3
*/

#include <NewSoftSerial.h>

NewSoftSerial lcd(2, 3);

void setup()
{
  Serial.begin(9600);  // start serial monitor serial
  lcd.begin(9600);  // start lcd serial
}

void loop()
{
  while(Serial.available())  // If serial data is available do this
    lcd.print((char) Serial.read());
}
