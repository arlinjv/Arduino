#include <EEPROMEx.h>
#include <SoftwareSerial.h>

// EEPROMex library downloaded from http://thijs.elenbaas.net/2012/07/extended-eeprom-library-for-arduino/ . also found here http://playground.arduino.cc/Code/EEPROMex

int currentReading;
int lowestReading = 1023;
int threshold = 350;
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

  EEPROM.setMaxAllowedWrites(maxAllowedWrites);
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
      newRecord = false;   // +++++++++++++++++++++>>>>>>>> need to fix this. causes 'done' to overwrite 'new record' 
      Serial.print("Writing new record to EEPROM: ");Serial.println(hangSeconds); // <<<<<<================= this doesn't seem to be writing to EEPROM. (EEPROM set program works fine though.
      bool goodWrite = EEPROM.writeFloat(recordTimeAddress, hangSeconds); 
      if (goodWrite) {
        float temp = EEPROM.readFloat(recordTimeAddress);
        Serial.print("Value read back from EEPROM: ");Serial.println(temp);
      }
      else {
        Serial.println("Bad Write");
      }
    }
    
    break;  // end case done:
    
  } //end switch
  
  // wait a bit for the analog-to-digital converter
  // to stabilize after the last reading:
  delay(10);
  
} // end loop

 

