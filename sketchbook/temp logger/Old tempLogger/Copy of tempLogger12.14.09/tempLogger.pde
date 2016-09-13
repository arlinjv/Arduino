#include <DateTimeStrings.h>
#include <DateTime.h>
#include <EEPROM.h>
#include <OneWire.h>

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
          Next steps:
              - Fix so that logging can occur while not tethered to laptop. disconnecting seems to cause reset. maybe add physical switch to cause logging on reset.
                  -- will need some way of updating time - may need to look into getting rtc chip.
              - Fix to allow stopping log without resetting (i.e. before memory full)
              - Change getTemps to a more generalizable form and simplify logTemps routine in memory tab
              - 
              Address bugs:
                 - spooky business where highbyte appears to increase by 4 each time through while logging. strangely data at address appears OK
                 problem not alleviated by commenting out TReading code. originally thought it was the << that was causing the problem
                 test by getting lower temp - see if highbyte changes to Zero as it should. if so may be OK to ignore bug.
    
              store time and intervals to EEPROM
              dump EEPROM data on prompt
              setTimeManual no longer works with puTTY - fix
              think about changing supermenu to start lgging at last memory spot automatically after fixed delay - will allow for power interruptions
 */


byte deviceCount; // initialized below in setup(). had to put this here so it would be recognized across tabs - it is used in getTemps. probably should pass into function
unsigned int interval = 0; //default delay is 0 - may want to change this, but should be good for testing purposes
boolean setInterval(unsigned int &_interval); // manual prototype added here so I can pass interval by reference later on


void setup() {
  Serial.begin(9600);
  
  superMenu(); // manual or auto mode?
  
  time_t arduinoTime = (946681200 + 3600); // first number supposed to be 0000 hours year 2000, yet apparently is 2300 prev. night
  DateTime.sync(arduinoTime); // available() returns false until sync done - probably could have just left this as zero
  digitalClockDisplay();        

 
  // Setup thermometers
  deviceCount = enumerate();
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

