/*
* Experimenting with serial monitor.
* What I've learned:
*  Arduino serial monitor can't handle carriage returns and backspaces
*  HyperTerminal doesn't send keypresses but responds to CR and BS
*  Teraterm sends keypresses, though doesn't have a text box
*/

//declare variables here
#include "WProgram.h"
void setup();
void loop();
void fill();
int i;
char bs = 8;
char cr = 13;
char space = 32;
char tab = 9;
int incomingByte = 0;	// for incoming serial data


void setup()                    // run once, when the sketch starts
{
  Serial.begin(9600);
}

void loop()                     // run over and over again
{
  if (Serial.available() > 0) {
	// read the incoming byte:
	incomingByte = Serial.read();
	// say what you got:
        Serial.print(char(incomingByte), BYTE);
        fill();
        Serial.print(incomingByte, BYTE);
        fill();
        Serial.print(incomingByte);
        fill();
  }
}

void fill()
  {
    Serial.print(">>>");
  }
int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

