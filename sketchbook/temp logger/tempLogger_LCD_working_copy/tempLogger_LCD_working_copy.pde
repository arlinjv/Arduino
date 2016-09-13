#include <DateTimeStrings.h>
#include <DateTime.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <SoftwareSerial.h>

/*
*Temperature datalogging sketch
 *
 *Pseudocode: 
 *Print menu
 *wait for input
 *  prompt for hour
 *  prompt for minute
 *  prompt for interval
 *write time to first two EEPROM slots
 *write interval to third
 *log temperatures until full
 *  while waiting for input
 *    if input says to break
 *      discontinue logging 
 *Wait for prompt to dump data
 
 connections:
   - red wire to 5v
   - blk wire to gnd
   - grn wire to pin 10
 
 progress6/11/09 - implemented function that gets hour and minute
 progress 6/12/09 - use hour and minute to set time and then display that time
 progress 6/13/09 - created menu function that (set time and interval, display time and interval)
 progress 6/26/09 - moved temp and menu functions to new tabs. added temp display to system info
 progress 6/26/09 - added init routine, and simple (debug) dump routine. added same to menu
 progress 6/27/09 - added logTemps routine and printLog routine. added test selection to menu
 progress 6/30/09 - fixed tempC to accept int temp reading parameter - allows use in other tabs.
                  - tested test routine at low temperature - works fine despite spooky output bug described below.
                  - Created supermenu: initial prompt to determine if access will be manual or automated (e.g. processing app)
                      - arduino waits for 'a' and dumps log or goes to menu
                      - superMenu() has a recursive element to it - undo this if program gets stuck
progress 12/14/09 - disabled reset on serial (added 120 ohm resistor across reset and 5v pins)
                    this allows arduino to continue operating after detaching from USB
progress 1/30/10 - corrected bug where logging resumes at last currentAddr (reset currentAddr inside beginLog
                 - added setStartDelay code to menu 
                 - added sensors to pshield
Progress 2/1/10  - added led code to begin log - on board yellow led is on during beginLog
                 - added and then disabled PWM code to beginLog. Idea was to change brightness of led depending on
                   number of measurements. unfortunately it didn't seem to make much difference.
Progress 3/17/10 - added LCD tab with setupLCD sub
Uploaded 3/17/10 1940, saved copy of folder to archive as tempLogger_LCD_3_17_10
Progress 3/17/10 - added LCD print routine to getTemps (need to clean up output.)
Uploaded 3/17/10 2315
Progress 3/18/10 - 
                 
          incomplete;
                - 
          Next steps:
              - add version tracking code (be nice to have name of file and date and time uploaded display on serial monitor)
              - make sure first measurement begins immediately (not after first delay)
              - Add display object to control status lights
                  - i.e. display.start(), display.numSensors() (blink once for each good sensor attached) etc.
              - add routine to blink display light once for each good sensor while waiting for input
              - Fix to allow stopping log without resetting (i.e. before memory full)
              - Change getTemps to a more generalizable form and simplify logTemps routine in memory tab
              - test with additional sensors
              Address bugs:
                 - spooky business where highbyte appears to increase by 4 each time through while logging. strangely data at address appears OK
                 problem not alleviated by commenting out TReading code. originally thought it was the << that was causing the problem
                 test by getting lower temp - see if highbyte changes to Zero as it should. if so may be OK to ignore bug.
    
              store time and intervals to EEPROM
              dump EEPROM data on prompt
              setTimeManual no longer works with puTTY - fix
              think about changing supermenu to start logging at last memory spot automatically after fixed delay - will allow for power interruptions
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
  
  superMenu(); // manual or auto mode?
  
  time_t arduinoTime = (946681200 + 3600); // first number supposed to be 0000 hours year 2000, yet apparently is 2300 prev. night
  DateTime.sync(arduinoTime); // available() returns false until sync done - probably could have just left this as zero
  digitalClockDisplay();    // menu tab    

  pinMode(ledPin, OUTPUT); // sets on board led pin (13) to output
  pinMode(pwmPin, OUTPUT);
 
  // Setup thermometers
  deviceCount = enumerate(); // getTemps tab
  Serial.print("device count is ");
  Serial.println(deviceCount,DEC);
  Serial.println("******************");

}    // End Setup


void loop() {


  if (!menu()) {                 // fancy way of calling menu
    Serial.println("try again"); // this code will not be called if valid selection entered
    Serial.flush();
    delay(100);
  }

}   // End Loop
//***************************************************************************************

