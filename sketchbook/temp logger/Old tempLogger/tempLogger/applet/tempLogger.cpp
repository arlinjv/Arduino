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

          Next steps:
              - add delay for first measurement
              - make sure first measurement begins immediately (not after first delay)
              - Move sensors to pshield (schematic at http://ladyada.net/images/pshield/v5schematic.png)
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


#include "WProgram.h"
void setup();
void loop();
byte enumerate();
void getTemps();
void printAddr();
float tempC(int TReading);
void beginLog();
void initEE();
void padEE();
boolean checkInterval();
void logTemps();
void printLog();
void dumpLog();
void superMenu();
boolean menu();
void digitalClockDisplay();
void printDigits(byte digits);
boolean setTimeManual();
int getDigit();
byte deviceCount; // initialized below in setup(). had to put this here so it would be recognized across tabs - it is used in getTemps. probably should pass into function
unsigned int interval = 0; //default delay is 0 - may want to change this, but should be good for testing purposes
boolean setInterval(unsigned int &_interval); // manual prototype added here so I can pass interval by reference later on


void setup() {
  Serial.begin(9600);
  
  superMenu(); // manual or auto mode?
  
  time_t arduinoTime = (946681200 + 3600); // first number supposed to be 0000 hours year 2000, yet apparently is 2300 prev. night
  DateTime.sync(arduinoTime); // available() returns false until sync done - probably could have just left this as zero
  digitalClockDisplay();    // menu tab    

 
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


//Think about storing temperatures as unsigned int - convert to binary before storing in eeprom then convert back
//If you declare a function with a two-word return type (e.g. "unsigned int") the environment will not realize 
//it's a function and will not create a prototype for it. That means you need to provide your own, or place the 
//definition of the function above any calls to it. 
//links:  http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1218921214
//        http://www.openobject.org/opensourceurbanism/Storing_Data


// DS18B20 Temperature chip i/o
//http://www.arduino.cc/playground/Learning/OneWire

int HighByte, LowByte, TReading, SignBit, Whole, Fract;
float Tc_100, Tf_100;

byte present = 0;
byte data[12];
byte addr[8];
OneWire ds(10);  // on pin 10



byte enumerate() {
  byte count = 0; 
  Serial.println("count\taddress");
  while (ds.search(addr)) {
    count++;
    Serial.print(count,DEC);
    Serial.print("\t");
    printAddr();
    Serial.println();
  }
  return count;
}


void getTemps() {
  byte i,j;
  ds.reset_search();
  for (i = 0; i < deviceCount; i++) {
    ds.search(addr);
    Serial.print(i,DEC);
    Serial.print("-");
    printAddr();
    Serial.print(", ");
    
    ds.reset();                 // Returns 1 if a device asserted a presence pulse, 0 otherwise.
    ds.select(addr);            // chooses the device with rom equal to address (0x55)
    ds.write(0x44,1);           // start conversion, with parasite power on at the end
    delay(1000);      // maybe 750ms is enough, maybe not
                    // we might do a ds.depower() here, but the reset will take care of it.
  
    present = ds.reset();
    ds.select(addr);    
    ds.write(0xBE);         // Read Scratchpad
 
  
    for ( j = 0; j < 9; j++) {           // we need 9 bytes
      data[j] = ds.read();
    }
    LowByte = data[0];
    HighByte = data[1];
    TReading = (HighByte << 8) + LowByte; // for processing sketch remember max value here is 7FFh, anything larger is due to (neg) sign bits
    Serial.print("H"); Serial.print(HighByte,HEX); Serial.print(" L");Serial.print(LowByte,HEX);Serial.print("=>");
    Serial.print(TReading);  // lose this and use data[0] and data[1] for eeprom
    Serial.print(" ");
    Serial.print(tempC(TReading));
    Serial.print("C");
    Serial.print("\t");
  }
  Serial.println();
}
 
void printAddr() {
  byte bitCount = 8;
  byte i;
  for (i = 0; i < bitCount; i++) {
    Serial.print(addr[i],HEX);
  }
}

float tempC(int TReading) {
   Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25, the resolution of the 18b20
   return (Tc_100/100);
}

 /*
  if ( !ds.search(addr)) {  //returns a 1 if next device has been enumerated. 0 if no more or some error
       ds.reset_search();   //reset_search required to start another search from the beginning
       return;
  }
 
 
  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.print("CRC is not valid!\n");
      return;
  }
 
  if ( addr[0] != 0x28) {
      Serial.print("Device is not a DS18S20 family device.\n");
      return;
  }
 
 // Tell device to start measuring temp
  ds.reset();                 // Returns 1 if a device asserted a presence pulse, 0 otherwise.
  ds.select(addr);            // chooses the device with rom equal to address (0x55)
  ds.write(0x44,1);           // start conversion, with parasite power on at the end
 
  delay(1000);      // maybe 750ms is enough, maybe not
                    // we might do a ds.depower() here, but the reset will take care of it.
 
 //get result of temp conversion
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad
 
  
    for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }
  
  LowByte = data[0];
  HighByte = data[1];
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;  // test most sig bit
  if (SignBit) // negative
  {
    TReading = (TReading ^ 0xffff) + 1; // 2's comp
  }
  Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25, the resolution of the 18b20
 
  // now convert to F by multiplying C by 1.8 and adding 32
  Tf_100 = Tc_100*9/5 + 3200;
  
 
  if (SignBit) // If its negative
  {
     Serial.print("-");
  }
  
*/

// EEPROM code goes here.
// EEPROM include needed on main tab
// EEPROM capacity is 512 bytes addressed from 0 to 511
// EEPROM.write(address, value)
//    address: the location to write to, from 0 to 511 (int) 
//    value: the value to write, from 0 to 255 (byte)
/*EEPROM memory layout: 
  0:  hour
  1:  minute
  2:  recording interval (minutes)
  3:  device count
  4 - 511:  data
  
*/

// setup EEPROM

const byte HOUR_ADDR = 0;
const byte MINUTE_ADDR = 1;
const byte INTERVAL_ADDR = 2;
const byte DEVICE_COUNT_ADDR = 3;
const byte DATA_START = 4; // good to start on even number so last element not left undefined
const int LAST_DATA_ADDR = 511;
const unsigned long MINUTE_IN_MILLIS = 60000;

unsigned long previousMillis = 0;
int currentAddr;// = DATA_START; - moved to beginLog
boolean fullEE = false;

void beginLog() {
  currentAddr = DATA_START;
  initEE();
  padEE();
  Serial.flush();
  while ( !(Serial.available()) ) { // attach serial reset disable jumper to continue logging after disconnect
    if ( checkInterval() ) {
      logTemps();
    }
    if (fullEE) {
      break;
    }
  }
  Serial.println("logging discontinued");
  Serial.flush();
}
// end beginLog **********************************************************************

void initEE() {
  if (DateTime.available()) {
    EEPROM.write(HOUR_ADDR,byte(DateTime.Hour));
    EEPROM.write(MINUTE_ADDR,byte(DateTime.Minute));
  } else {
    EEPROM.write(HOUR_ADDR,255);  // 255 indicates bad hour or minute
    EEPROM.write(MINUTE_ADDR,255);
  }
  EEPROM.write(INTERVAL_ADDR,interval);
  EEPROM.write(DEVICE_COUNT_ADDR,deviceCount); // address three left blank for now so that memory fills out evenly
} // end initEE

void padEE() {
  int i;
  for (i = DATA_START; i <= LAST_DATA_ADDR; i++) {
    EEPROM.write(i,0);
  } 
}// end padEE

boolean checkInterval() {
  if (millis() - previousMillis > (interval*MINUTE_IN_MILLIS)) {
    previousMillis = millis();
    return true;
  }
  else {
    return false;
  } 
}  // end checkInterval

void logTemps() { //really should have created a seperate temp class, now i have to repeat most of the getTemps code here
//                  write high and low bytes to memory for each reading. called once for each interval
  byte present = 0;
  byte i,j;
  int HighByte = 0;
  int LowByte = 0;
  int TReading;
  byte data[12];
  byte addr[8];
  OneWire ds(10);  // on pin 10
  
  ds.reset_search();
  for (i = 0; i < deviceCount; i++) {
    ds.search(addr);
    ds.reset();                 // Returns 1 if a device asserted a presence pulse, 0 otherwise.
    ds.select(addr);            // chooses the device with rom equal to address (0x55)
    ds.write(0x44,1);           // start conversion, with parasite power on at the end
    delay(1000);      // maybe 750ms is enough, maybe not
                    // we might do a ds.depower() here, but the reset will take care of it.
  
      
    present = ds.reset();
    ds.select(addr);    
    ds.write(0xBE);         // Read Scratchpad
    
    for ( j = 0; j < 9; j++) {           // we need 9 bytes
      data[j] = ds.read();
    }
    
    LowByte = data[0];
    HighByte = data[1];
    TReading = (HighByte << 8) + LowByte;    
    
    if (currentAddr <= (LAST_DATA_ADDR - 1)) {
      EEPROM.write(currentAddr,HighByte);
      Serial.print(currentAddr,DEC); Serial.print(" H"); Serial.print(HighByte,HEX);
      currentAddr++;
      EEPROM.write(currentAddr,LowByte);
      Serial.print(currentAddr,DEC);  Serial.print(" L");  Serial.print(LowByte,HEX); Serial.print(" ");
      Serial.print ("=>"); Serial.print(TReading); Serial.print("\t");
      currentAddr++;
    }
    else {
      fullEE = true;
      Serial.print("memory full. stopped at address:");
      Serial.println( (currentAddr-1),DEC);
      break;
    }
  }
  Serial.println();
} // end logTemps*******************************************

void printLog() {
  int i,j;
  int HB, LB, TR;
  Serial.print("logging started at: ");
  Serial.print(EEPROM.read(0),DEC);
  Serial.print(":");
  Serial.print(EEPROM.read(1),DEC);
  Serial.print("\tinterval = ");
  Serial.print(EEPROM.read(2),DEC);
  Serial.print("\tdevice count (EEprom position 3) = ");
  Serial.println(EEPROM.read(3),DEC);
  
  for (i = DATA_START; i <= (LAST_DATA_ADDR - 1); ) {   // i is incremented below
    for (j = 0; j < deviceCount; j++ ) {
      Serial.print(i,DEC); Serial.print("-");
      HB = EEPROM.read(i++);
      Serial.print(HB,HEX);
      Serial.print(" ");
      LB = EEPROM.read(i++);
      Serial.print(LB,HEX);
      TR = (HB << 8) + LB;
      Serial.print(","); Serial.print(tempC(TR));
      Serial.print("\t");
    }
    Serial.println();
  }
  Serial.println();
}  // end printLog ***********************************

void dumpLog() {
  int i;
  for (i = 0; i <= LAST_DATA_ADDR; i++) {
    Serial.print( EEPROM.read(i),BYTE );
  }
  Serial.println();
}


void superMenu() { // determine if manual or auto mode, called at beginning of setup
  while (!Serial.available() ) {
    //Serial.print('.'); this was nice for creating prompt for manual users but makes processing program too difficult
    //delay(500);
} // loop until receive input while printing periods
  char selection = Serial.read();
  if (selection == 'a') {
    dumpLog();
    Serial.flush();
    superMenu();  // 
  }
}


boolean menu() {
  Serial.println("-----Enter s to set time, m to set interval(minutes), d to display system info (time, interval, temperatures ...,");
  Serial.println("     t to test, L to begin logging, O to output EEPROM");
  while (!Serial.available()){
  }
  char selection = Serial.read();
  Serial.println(selection);
  if (selection == 's') {
    if ( setTimeManual() )  {
      return true;
    }
  } 
  else if (selection == 'd') {   // display system info
    digitalClockDisplay();
    Serial.print("interval is set to ");
    Serial.print(interval);
    Serial.println(" minutes");
    getTemps();
    return true;
  } 
  else if (selection == 'm') {
    if ( setInterval(interval) ) {
      return true;
    }
  }
  else if (selection == 'L') {
    beginLog();
    return true;
  }
  else if (selection == 'O') {
    printLog();
    return true;
  }
  else if (selection == 't') {  // test option
    getTemps();
    logTemps();
    return true;
  }  
  return false;
}

void digitalClockDisplay(){
  // digital clock display of current date and time
  DateTime.available(); // must do .sync() first, otherwise returns zero (?)
  Serial.print(DateTime.Hour,DEC);
  printDigits(DateTime.Minute);
  printDigits(DateTime.Second);
  Serial.print(" ");
  Serial.print(DateTimeStrings.dayStr(DateTime.DayofWeek));
  Serial.print(" ");
  Serial.print(DateTimeStrings.monthStr(DateTime.Month));
  Serial.print(" ");
  Serial.println(DateTime.Day,DEC);
}
void printDigits(byte digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits,DEC);
}


boolean setTimeManual() {
  long hour;
  long minutes;
  Serial.println("Enter time - hhmm");
  hour = getDigit();
  hour = hour*10 + getDigit();
  if (!(hour >= 0 && hour <= 23)) {
    return false;
  } 
  minutes = getDigit();
  minutes = minutes*10 + getDigit();
  if (!(minutes >=0 && minutes<=59)) {
    return false;
  }
  Serial.print(hour); 
  printDigits(minutes);
  Serial.println();

  unsigned long newTime = (946681200 + 3600L) + minutes*60L + hour*3600L; 
  DateTime.sync(newTime);
  return true;
}    // End setTimeManual

boolean setInterval(unsigned int &_interval) {
  unsigned int temp = 0;
  Serial.println("Enter the logging interval in minutes - xxx (pad front with zeroes)");
  temp = getDigit();
  temp = temp*10 + getDigit();
  temp = temp*10 + getDigit();
  _interval = temp;
  Serial.print("Logging interval set to ");
  Serial.print(_interval);
  Serial.println(" minute(s)");
  return true;
}

int getDigit() {
  int i;
  while (!Serial.available()){
  }
  char c= Serial.read();
  if (c>= '0' && c<= '9') {
    i = c - '0';
  }
  return i;
}

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

