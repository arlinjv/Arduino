/*

Writing some code from scratch to test the blue background 16x2 module from NKC
LCDPins:    Arduino:
  1  GND      
  2  VDD
  3  V0              (contrast adjust)
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
  15 A       13     (LED +, I_led = 120mA Vf = 4.2v  220ohms gives 20mA)
  16 K       GND  (LED -)
*/



#include "WProgram.h"
void setup();
void loop();
void getSerial();
void backlightOff ();
void backlightOn ();
const int RS = 2;
const int RW = 3;
const int E = 4;
const int D4 = 5;
const int D5 = 6;
const int D6 = 7;
const int D7 = 8;
const int LED = 13;

int keyIn = 0;

void setup()                    // run once, when the sketch starts
{
  Serial.begin(9600);
  backlightOff();
}

void loop()                     // run over and over again
{
  getSerial();
}

void getSerial()
{
  if (Serial.available() > 0)
  {
    keyIn = Serial.read();  
    Serial.print(keyIn, BYTE);
    if (keyIn == 'L')
    {
      backlightOn();
      Serial.println("backlight on");
    }
    else if (keyIn == 'l')
    {
      backlightOff();
      Serial.println("backlight off");
    }
    else
    {
      Serial.println("options: L - Led On; l - Led Off;");
    }
  }
}
  
 
  
void backlightOff ()
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
}  
void backlightOn ()
{
  pinMode(LED, OUTPUT);
  digitalWrite(LED, HIGH);
}  
int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

