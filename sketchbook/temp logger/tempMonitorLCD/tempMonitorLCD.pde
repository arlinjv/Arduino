#include <DateTimeStrings.h>
#include <DateTime.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <SoftwareSerial.h>

/*
Continuously makes temperature readings and writes to LCD
 */


byte deviceCount; // initialized below in setup(). had to put this here so it would be recognized across tabs - it is used in getTemps. probably should pass into function
unsigned int interval = 0; //default delay is 0 - may want to change this, but should be good for testing purposes
unsigned int startDelay = 0;
boolean setInterval(unsigned int &_interval); // manual prototype added here so I can pass interval by reference later on
boolean setStartDelay(unsigned int &_startDelay);

int ledPin = 13;
int pwmPin = 11;

void setup() {
  Serial.begin(9600);
  
  setupLCD();
  

  
  time_t arduinoTime = (946681200 + 3600); // first number supposed to be 0000 hours year 2000, yet apparently is 2300 prev. night
  DateTime.sync(arduinoTime); // available() returns false until sync done - probably could have just left this as zero


 
  // Setup thermometers
  deviceCount = enumerate(); // getTemps tab
  Serial.print("device count is ");
  Serial.println(deviceCount,DEC);
  Serial.println("******************");

}    // End Setup


void loop() {

getTemps();
delay(1000);

}   // End Loop
//***************************************************************************************

