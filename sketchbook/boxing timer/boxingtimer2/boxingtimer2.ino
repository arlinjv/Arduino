#include <Button.h>

//  Hardware setup:  One rocker switch for power. Two LED indicator lights - red (paused) and green (running). One momentary push button (start).
//                  Two single pole rotary switches each of which is wired to a resistor ladder. The resistor ladder connects via the switch to an analog input.
//  Use case 1:                                                Use Case 2: (Timer is running and user changes round or rest time)
//  1) User turns power on using ON/OFF switch                  1) User selects a new round or rest time
//  2) Red indicator light turns on                             2) Timer stops. LED goes from Green to Red
//  3) User selects round length using rotary switch            3) User pushes START button to restart the timer
//  4) User selects rest length using rotary switch             4) Go to Use Case 1 line 6.
//  5) User presses start button
//  6) Bell rings once to start round
//  7) Green LED turns on to indicate timer is running
//  8) Round finishes.
//  9) Bell rings three times
//  10) Green LED starts blinking 
//  11) Rest period finishes
//  12) return to 6



// To do:
//    Intregrate testing menu add jumper to board to determine testing or running mode
//    - allow user to select pulse length and interval and save to EEPROM
//    - self test to check for proper hardware installation
//    - resistor value check routine that prompts user to change switch settings. save adjusted values in EEPROM
//    consider adding stop function for start button (currently need to change one of the rotator switches to leave round/rest loop)
//      - will probably need to improve debouncing (current delay(10) doesn't really cut it.
//    modify blinking routine to have light phase in and out
//    consider using bi-color LED to enable use of only one LED. maybe even move that LED into switch.
//    consider adding some sort of error checking for switch readings (maybe by check for specific ranges or by taking multiple readings. mostly cause of error would be if wire comes loose)


const int runningLED = 13; //green LED (built in LED for now. change to pin 10 for mockup)
const int pauseLED = 12; // red LED
const int solenoidPin = 10;
const int startButtonPin = 11;
const int roundLengthPin = A0;
const int restLengthPin = A1;
const int testModePin = 2;
const int pulseLength = 20;
const int pulseInterval = 225;

int roundLengthSelection = 0; //0 value causes loop to go straight to pause state
int restLengthSelection = 0;

unsigned long roundLength; // in milliseconds
unsigned long restLength; 
unsigned long endOfRound;
unsigned long endOfRest;

boolean testMode = false;

Button startButton = Button(startButtonPin,BUTTON_PULLUP_INTERNAL); 

enum states { 
  pause,
  start,
  in_round,
  rest_period
};

states state = pause; 

// the setup routine runs once when you press reset:
void setup() {

  pinMode(solenoidPin, OUTPUT);
  digitalWrite(solenoidPin, LOW);
  
  pinMode(runningLED, OUTPUT);
  digitalWrite(runningLED, LOW);
  
  pinMode(pauseLED, OUTPUT);
  digitalWrite(pauseLED, LOW);
  
  pinMode(testModePin, INPUT);
  digitalWrite(testModePin, HIGH); // turn on internal pullup resistor
  
  Serial.begin(9600);
  Serial.println("ready");
}

// the loop routine runs over and over again forever:
void loop() {
  
  checkSwitches();  // every loop check for changes in switch states
  
  switch(state){
    
    case pause: // powering the unit up doesn't go straight to bell - have to push start button to get things going. (see check inputs)

      digitalWrite(pauseLED, HIGH); // red LED on
      digitalWrite(runningLED, LOW); // green LED off

      // check start button
      if (startButton.uniquePress()) {
        state = start;
        delay(10); // uniquePress seems a little glitchy (not a very effective debounce). this should cover any bounces
      }  

      break;    
    
    case start: //read switch settings and then begin round. get here either by hitting start button or when rest period ends

      digitalWrite(pauseLED, LOW); // red LED off
      
      Serial.println("start");
      roundLength = getRoundLengthMillis(roundLengthSelection);
      //roundLength = 60000; 
      if (roundLength == 0){ // this shouldn't happen - currently no error checking mechanism in readRotatorSwitch()
        state = pause;
        Serial.println("something is wrong.");
        break;
      }
      
      restLength = getRestLengthMillis(restLengthSelection); 
      if (restLength == 0){ // see above
        state = pause;
        Serial.println("something is wrong.");
        break;
      }
      
      Serial.print("round length in milliseconds: "); Serial.println(roundLength);
      Serial.print("rest length in milliseconds: "); Serial.println(restLength);
      endOfRound = millis() + roundLength;
      endOfRest = endOfRound + restLength;

      state = in_round;

      ding(); // single ding to announce beginning of round
      break;

    case in_round:
      
      digitalWrite(runningLED, HIGH); // solid green LED
      if ((long)(millis() - endOfRound) >= 0){ //this approach should cover rollover situation. (see http://playground.arduino.cc/Code/TimingRollover )
        state = rest_period;
        Serial.print("Round over. Timing error = "); Serial.println((long)(millis() - endOfRound));
        //bell(); // ******* add triple buzz/ding here ******** // signifies end of round
        tripleDing(); // three rings of the bell to announce end of round
      }      
      break;
      
    case rest_period:
      
      blinkRunningLED();
      
      if ((long)(millis() - endOfRest) >= 0){
        state = start;
        Serial.print("Rest over. Timing error = "); Serial.println((long)(millis() - endOfRest));       
      }
      
      break;

  } // end switch
     
} // end loop()

//******************************************************
void checkSwitches(){

  // test mode enabled by connecting digital pin 2 to ground
  boolean oldTestModeState = testMode;  
  if (digitalRead(testModePin) == LOW) {
    testMode = true;
  }
  else {
    testMode = false;
  }  
  if (oldTestModeState != testMode) {
    state = pause;
    Serial.println("test mode toggled");
  }
  
  // check for change in round length setting - go to pause if changed
  int oldRoundLengthSelection = roundLengthSelection;
  int newRoundLengthSelection = readRotatorSwitch(roundLengthPin);
  
  if (oldRoundLengthSelection != newRoundLengthSelection) {
    roundLengthSelection = newRoundLengthSelection;
    Serial.print("New round length selection = "); Serial.println(roundLengthSelection);
    state = pause;
  }
  
  // check for change in rest length setting - go to pause if changed
  int oldRestLengthSelection = restLengthSelection;
  int newRestLengthSelection = readRotatorSwitch(restLengthPin);
  
  if (oldRestLengthSelection != newRestLengthSelection) {
    restLengthSelection = newRestLengthSelection;
    Serial.print("New rest length selection = "); Serial.println(restLengthSelection);
    state = pause;
  }
  
} // end checkSwitches

//******************************************************
unsigned long getRoundLengthMillis(int selection){

  if (testMode == true) {
    return 10000;
  }
  
  switch(selection){
    case 0:
      Serial.println("bad reading");
      return(0); //      
    case 1:
      return(60000); //one minute
    case 2:
      return(120000); // two minutes
    case 3:
      return(180000);
    case 4:
      return(2400000);
    case 5:
      return(300000);
    case 6:
      return(300000); // currently only five settings desired so if six comes up should be treated same as 5
    
    // implement default as error catcher?
  } // end switch
  
} //end getRoundLengthMillis()

//******************************************************
unsigned long getRestLengthMillis(int selection){
  
  if (testMode == true){
    return 5000;
  }
  
  switch(selection){
    case 0:
      Serial.println("bad reading");
      return(0); // 
    case 1:
      return(15000); //15 seconds
    case 2:
      return(30000); // 30 seconds
    case 3:
      return(45000);
    case 4:
      return(60000);
    case 5:
      return(120000);
    case 6:
      return(180000);
    
  } // end switch
  
} //end getRestLengthMillis()

//******************************************************
int readRotatorSwitch(int switchPin){
  //Experimental switch values from six element 740 ohm resistor ladder: 0, 196, 392, 590, 787, 983, // old: 0, 168, 340, 511, 682, 853, 1023
  //change analogVals and valCount to reflect current hardware setup.
  //Assumes identical setup for any switches to be read
  
  int analogVals[] = {0, 196, 392, 590, 787, 983};
  int valCount = 6; // = length of analogVals. used for iterator below. could use sizeof() but this is easier to read
  int selection = 0; // return value of 0 indicates bad reading - shouldn't happen. the method below doesn't check for problems 
  int switchReading = analogRead(switchPin);
  
  for(int val = 0; val < valCount; val++){
    if (switchReading >= (analogVals[val] - 50)){ //50 represents arbitrary sensitivity setting. on breadboard readings vary no more than +/- 1
      selection++;
    }
  }

  //Serial.print("analog reading at selection ");  Serial.print(selection);  Serial.print(": ");  Serial.println(switchReading);
  
  return selection;
  
} // end readRotatorSwitch()


void blinkRunningLED(){
  
  const int blinkTime = 250;
  static unsigned long nextToggleTime; // start value doesn't matter since we will toggle anyway
  
  if ((long)(millis() - nextToggleTime) >= 0){
    digitalWrite(runningLED, !digitalRead (runningLED)); 
    nextToggleTime = millis() + blinkTime;
  }
}

void ding(){
  
  digitalWrite(solenoidPin, HIGH);
  delay(pulseLength);
  digitalWrite(solenoidPin, LOW);
  Serial.println("ding");
  
}

void tripleDing(){

  int i;
  for (i=0; i < 3; i++){
    digitalWrite(solenoidPin, HIGH);
    delay(pulseLength);
    digitalWrite(solenoidPin, LOW);
    delay(pulseInterval);
    Serial.println("ding");
  }
}
