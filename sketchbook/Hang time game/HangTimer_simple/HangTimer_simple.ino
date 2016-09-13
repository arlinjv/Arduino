/*

Hardware:
  - platform is Sparkfun Serial Enabled LCD Kit (LCD-10097) which consists of LCD and Arduino based LCD backpack kit
  - Sparkfun nav switch (COM-08236) installed across pins D10 - D13
  - pressure sensor consisting of electrostatic dissipative foam based variable resistor in voltage divider with a 10k resistor

// EEPROMex library downloaded from http://thijs.elenbaas.net/2012/07/extended-eeprom-library-for-arduino/ . also found here http://playground.arduino.cc/Code/EEPROMex

Status: seem to have run out of RAM when implementing name list with records save for each.
        not sure if the program was entirely debugged but in areas where it appears to work, previously working lcd strings are now corrupted
        reducing name size didn't help

To do:
  - implement auto power off
  - implement pwm control of LCD backlight to save batteries
  - implement records list in EEPROM which will store record time and initials for top ten scores (use struct?)
  - get rid of all uses of String
  
Done:
  

*/

#include <Button.h>
#include <EEPROMEx.h>
#include <LiquidCrystal.h>

//Interface pins for the SparkFun Serial LCD Kit
const int rsPin = 2;
const int rwPin = 3;
const int enablePin = 4;
const int d4 = 5;
const int d5 = 6;
const int d6 = 7;
const int d7 = 8;
LiquidCrystal lcd(rsPin,rwPin,enablePin,d4,d5,d6,d7);

// interface for nav switch
const int nSwitchLeftPin = 13;
const int nSwitchCenterPin = 12;
const int nSwitchRightPin = 11;
const int nSwitchCommon = 10;
Button lButton = Button(nSwitchLeftPin,BUTTON_PULLUP_INTERNAL);
Button rButton = Button(nSwitchRightPin,BUTTON_PULLUP_INTERNAL);
Button cButton = Button(nSwitchCenterPin,BUTTON_PULLUP_INTERNAL);


const int buzzerPin = 19; //A5
int buzzCount = 0;

int currentReading;
int lowestReading = 1023;
const int threshold = 350;
unsigned long startTime;
unsigned long endTime;
int lowCount = 0; // number of successive analog reads below threshold
float hangSeconds = 0; // number of seconds person holds on bar
int prevSeconds = 0;

boolean newRecord = false;

float recordTime;
int recordTimeAddress;  // EEPROM address of record time. initilized in setupEE()

String lineOne = "";
String lineTwo = "";

String sketchVersion = "updated 5.11.13";


enum states { 
  standby, //implement later. have lights off. maybe low power mode. 
  welcome,
  startup, // for getting stable reading
  ready,
  counting,
  done,
  hold
};

states state = ready;

void setup() {

  Serial.begin(115200);
    
  setupNavSwitch();
  setupLCD();               //
  setupEE();                // check for reset then get userIndex and recordTime from EEPROM
  
} // end setup()

void loop() {

  currentReading = analogRead(A2); // analog pin A2
  
  if (currentReading < lowestReading) {
    lowestReading = currentReading;
  }
  
  updateLCD();
  updateBuzzer();
  //timer.update(); // for to do list: add the implementation of timer class. would help greatly to clean up 'counting' case
  
  
  switch(state) {
    // ----------------------------
    case standby: //currently no function other than for debugging
      break;
      
    // ----------------------------
    case welcome:
      // previously contained select user dialog. see 5_10_13 version
      break;

    // ----------------------------      
    case ready:
      newRecord = false; 
      recordTime = EEPROM.readFloat(recordTimeAddress); // get record time 
      //Serial.print("Record time = "); Serial.println(recordTime);
      
      lineOne = "Ready...........";
      lineTwo = String(currentReading);
      
      if (currentReading > threshold) { //still waiting or previous low value was a glitch or just a tug on the bar. keep waiting.
        lowCount = 0;
        hangSeconds = 0;
        break;
      }

      //Serial.print("lowCount = "); Serial.println(lowCount); 
      
      if (lowCount > 10) {
        lowCount = 0;
        state = counting;
        //Serial.println("'ready'-> 'done' ");Serial.print("startTime (millis) = "); Serial.print(startTime); Serial.print(" currentReading = "); Serial.println(currentReading);
        break;
      }
      if (lowCount == 0) { // base start time on first occurrence of below threshold value 
        startTime = millis();
      }  
      
      lowCount++;

      break; // end case ready:

  // ----------------------------
  case counting:
    
    lineOne = "Counting........";

    prevSeconds = int(hangSeconds); // truncate hangSeconds by converting to int. use for new second checking

    endTime = millis();
    hangSeconds = endTime - startTime;
    hangSeconds = hangSeconds/1000;  // convert from milliseconds to seconds
    
    // Check for new second:      (would be nice to implement this in a timer class. something like if(timer.newSecond) ...
    if (prevSeconds < int(hangSeconds)){ // compare integer value of prevSeconds to truncated value of hangSeconds
      Serial.println(hangSeconds);
      buzzCount += 1; // single beep to mark new second
    }
    
    // Check for new record      (replace someday with if(timer.newRecord) ...
    if (newRecord == false && hangSeconds > recordTime){ 
      newRecord = true;
      //Serial.println("NR");
      if (recordTime > 0){// check for zero record time so buzzer doesn't go off when records have been reset
        //Serial.print("RT = "); Serial.println(recordTime);
        buzzCount += 10; // ten beeps
      }
    }

    char digits[10];
    dtostrf(hangSeconds, 5, 1, digits); // see http://arduino.cc/forum/index.php?topic=85523.0 for discussion of dtostrf. needed because sprintf can't handle floats in arduino
    lineTwo = digits;
    
    if (currentReading < threshold) { // still hanging in there
      break;
    }
    else {
      state = done; // pressure reading is > threshold so skip to next state
      break;
    }      
  // end case counting:
        
  // ----------------------------
  case done:
    //Serial.print("done. ");Serial.println(hangSeconds);
    state = hold;
    
    if (newRecord == true) { 
      lineOne = "New Record!!!   ";
      recordTime = hangSeconds;
      
      // lineTwo value should remain as last set in counting case
      //Serial.print("Writing new record to EEPROM: ");Serial.println(hangSeconds); 
      EEPROM.updateFloat(recordTimeAddress, recordTime); 
    }
    else {
      lineOne = "Done....        ";
      // lineTwo should remain as last set in counting case
    }
      
    break;  // end case done:
    
  // ----------------------------  
  case hold:               // To do: implement auto power-off
    if(cButton.uniquePress() || (millis() > endTime + 5000)){ // press center button to go again or wait 5 seconds
      state = ready;
      break;
    }
    break; // end case hold:
    
  } //end switch
  
  delay(50); 
  
} // end loop()


