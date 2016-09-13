/*
Sketch to test liquidcrystal library
LCDPins:    Arduino:
  1  GND      
  2  VDD
  3                (contrast adjust)
  4  RS      13    (register select)
  5  RW      12
  6  E       11
  7  DB0     10
  8  DB1     9
  9  DB2     8
  10 DB3     7
  11 DB4     6
  12 DB5     5
  13 DB6     4
  14 DB7     3
  15 A
  16 K
*/

#include <LiquidCrystal.h>

const int RS = 13;
const int RW = 12;
const int E = 11;
const int D0 = 10;
const int D1 = 9;
const int D2 = 8;
const int D3 = 7;
const int D4 = 6;
const int D5 = 5;
const int D6 = 4;
const int D7 = 3;

LiquidCrystal lcd(RS, RW, E, D0, D1, D2, D3, D4, D5, D6, D7);

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
