/*
Sketch to test liquidcrystal library
Using the blue background 16x2 module from NKC
LCDPins:    Arduino:
  1  GND      
  2  VDD
  3  V0  
  4  RS      2
  5  RW      3
  6  E       4
  7  DB0      
  8  DB1
  9  DB2
  10 DB3
  11 DB4     5
  12 DB5     6
  13 DB6     7
  14 DB7     8
  15 A
  16 K
*/

#include <LiquidCrystal.h>

const int RS = 2;
const int RW = 3;
const int E = 4;
const int D4 = 5;
const int D5 = 6;
const int D6 = 7;
const int D7 = 8;

LiquidCrystal lcd(RS, RW, E, D4, D5, D6, D7);

void setup()                    // run once, when the sketch starts
{
 Serial.begin(9600);
 lcd.print("hello, world");
 Serial.print("starting");
 }

void loop()                     // run over and over again
{
  Serial.println("still here");
  delay(1000);
}
