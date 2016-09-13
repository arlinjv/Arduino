#include <DateTimeStrings.h>
#include <DateTime.h>
#include <EEPROM.h>
#include <OneWire.h>
#include <SoftwareSerial.h>

/*

 */


#include "WProgram.h"
void setup();
void loop();
void setupLCD ();
void clearLCD();
void selectLineOne();
void selectLineTwo();
byte enumerate();
void getTemps();
void printAddr();
float tempC(int TReading);
float tempF();
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


//using Sparkfun serial lcd, partno LCD-00461. Appears to be discontinued.
//Reference:
//    http://www.sparkfun.com/datasheets/LCD/SerLCD_V2_5.PDF (pdf saved in program directory
//    http://www.arduino.cc/playground/Learning/SparkFunSerLCD


int txPin = 2;

SoftwareSerial LCD = SoftwareSerial(0, txPin);
// since the LCD does not send data back to the Arduino, we should only define the txPin

void setupLCD () {
  pinMode(txPin, OUTPUT);
  LCD.begin(9600);
  clearLCD();
  LCD.print("LCD setup");
  delay(500);
  clearLCD();
}

void clearLCD(){
   LCD.print(0xFE, BYTE);   //command flag
   LCD.print(0x01, BYTE);   //clear command.
}

void selectLineOne(){  //puts the cursor at line 0 char 0.
   LCD.print(0xFE, BYTE);   //command flag
   LCD.print(128, BYTE);    //position
}
void selectLineTwo(){  //puts the cursor at line 1 char 0.
   LCD.print(0xFE, BYTE);   //command flag
   LCD.print(192, BYTE);    //position
}

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
  clearLCD();
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
    LCD.print(int( tempF() ) ); //can't print floats with software serial
    if ( i < deviceCount - 1 ) {
      LCD.print(", ");
    }
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

float tempF() {
  Tf_100 = Tc_100*9/5 + 3200; // bit ugly here - this routine only works if tempC is called first
  return (Tf_100/100);
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

int main(void)
{
	init();

	setup();
    
	for (;;)
		loop();
        
	return 0;
}

