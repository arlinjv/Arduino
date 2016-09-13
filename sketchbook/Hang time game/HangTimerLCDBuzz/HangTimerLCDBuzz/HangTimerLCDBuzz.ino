#include <EEPROMEx.h>
#include <SoftwareSerial.h>

// EEPROMex library downloaded from http://thijs.elenbaas.net/2012/07/extended-eeprom-library-for-arduino/ . also found here http://playground.arduino.cc/Code/EEPROMex

int currentReading;
int lowestReading = 1023;
int threshold = 325;
unsigned long startTime;
unsigned long endTime;
int lowCount = 0; // number of successive analog reads below threshold
float hangSeconds = 0; // number of seconds person holds on bar

boolean newRecord = false;

String lineOne = "";
String lineTwo = "";

float recordTime;
int recordTimeAddress = 0; // EEPROM address of record time.

enum states { 
  standby, //implement later. have lights off. maybe low power mode. 
  startup, // for getting stable reading
  ready,
  counting,
  done
};

states state = ready;

void setup() {
  Serial.begin(115200); //for debugging
  
  setupLCD();
  setupBuzzer();
  
  recordTime = EEPROM.readFloat(recordTimeAddress);
  Serial.print("Record time = "); Serial.println(recordTime);
    
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

      Serial.print("lowCount = "); Serial.println(lowCount); // for debugging
      
      if (lowCount > 10) {
        lowCount = 0;
        state = counting;
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
      
    if (currentReading < threshold) { // still hanging in there
      break;
    }
        
    state = done; 
    break; // end case counting:
        
  case done:
    if (newRecord == true) {
      EEPROM.writeFloat(hangSeconds, recordTimeAddress);
    }
 
    lineOne = "done............";
    break;  // end case done:
    
  } //end switch
  
  // wait a bit for the analog-to-digital converter
  // to stabilize after the last reading:
  delay(10);
  
} // end loop


  

