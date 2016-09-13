/*
Known bugs:
  - small discrepancy (~.1 to .2 seconds) between  displayed record time and recorded record time (can probably fix this while recrafting buzzer routines
  - unit freezes occassionally. apparently due to loss of memory. excessive serial debugging messages and (probably) use of Strings are culprits

To do:
  - revamp buzzer routines (create buzzer class)
  - remove Strings and replace with char arrays.
  

*/


#include <EEPROMEx.h>
#include <SoftwareSerial.h>
#include "SerLCD_Class.h"

// EEPROMex library downloaded from http://thijs.elenbaas.net/2012/07/extended-eeprom-library-for-arduino/ . also found here http://playground.arduino.cc/Code/EEPROMex


int currentReading;
int lowestReading = 1023;
const int threshold = 350;
unsigned long startTime;
unsigned long endTime;
int lowCount = 0; // number of successive analog reads below threshold
float hangSeconds = 0; // number of seconds person holds on bar

boolean newRecord = false;

String lineOne = "";
String lineTwo = "";

float recordTime;
const int recordTimeAddress = 10; // EEPROM address of record time. originally used 0 but think may have written to too many times
const int maxAllowedWrites = 20;

const int lcdPin = 2;
SerLCD lcd(lcdPin);

enum states { 
  standby, //implement later. have lights off. maybe low power mode. 
  startup, // for getting stable reading
  ready,
  counting,
  done,
  hold
};

states state = ready;

void setup() {
  Serial.begin(115200); //for debugging
  
  lcd.setupLCD();
  setupBuzzer();

  EEPROM.setMaxAllowedWrites(maxAllowedWrites);
  recordTime = EEPROM.readFloat(recordTimeAddress);
  Serial.print("Record time = "); Serial.println(recordTime);

  checkReset();    
}

void loop() {
  currentReading = analogRead(A0);
  
  if (currentReading < lowestReading) {
    lowestReading = currentReading;
  }
  
  updateLCD();
  updateBuzzer();
  
  switch(state) {
    case ready:
      lineOne = "Ready...........";
      if (currentReading > threshold) { //still waiting or previous low value was a glitch or just a tug on the bar. keep waiting.
        lowCount = 0;
        hangSeconds = 0;
        break;
      }

      //Serial.print("lowCount = "); Serial.println(lowCount); // for debugging
      
      if (lowCount > 10) {
        lowCount = 0;
        state = counting;
        //Serial.println("'ready'-> 'done' ");
        //Serial.print("startTime (millis) = "); Serial.print(startTime); Serial.print(" currentReading = "); Serial.println(currentReading);
        break;
      }
      if (lowCount == 0) {
        startTime = millis();
      }  
      
      lowCount++;

      break; // end case ready:

  case counting:
    lineOne = "Counting........";
    endTime = millis();
    hangSeconds = endTime - startTime;
    hangSeconds = hangSeconds/1000;
    
    char digits[10];
    dtostrf(hangSeconds, 5, 1, digits); // see http://arduino.cc/forum/index.php?topic=85523.0 for discussion of dtostrf. needed because sprintf can't handle floats in arduino
    lineTwo = digits;
    
    if (currentReading < threshold) { // still hanging in there
      break;
    }
    else {
      state = done; 
      break;
    }      
  // end case counting:
        
  case done:
    //Serial.print("done. ");Serial.println(hangSeconds);
    state = hold;
    
    if (newRecord == true) {
      lineOne = "New Record!!!   ";
      //Serial.print("Writing new record to EEPROM: ");Serial.println(hangSeconds); 
      bool goodWrite = EEPROM.updateFloat(recordTimeAddress, hangSeconds); 
      if (goodWrite) {
        float temp = EEPROM.readFloat(recordTimeAddress);
        //Serial.print("Value read back from EEPROM: ");Serial.println(temp);
      }
      else {
        //Serial.println("Bad Write");
      }
    }
    else {
      lineOne = "Done....        ";
    }
      
    break;  // end case done:
    
  case hold:
    //Serial.println("hold");
    break; // end case hold:
    
  } //end switch
  
  // wait a bit for the analog-to-digital converter
  // to stabilize after the last reading:
  delay(50); //changed to 50 for debugging. 10 worked fine during previous stable version
  
} // end loop

void checkReset() {
  pinMode(A3, INPUT_PULLUP);
  int val = digitalRead(A3);
  //Serial.print("reset pin state - ");Serial.println(val);
  if (val == 0){
    recordTime = 0;
  }
  
} // end resetRecord()

