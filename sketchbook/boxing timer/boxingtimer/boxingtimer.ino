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
//  9) Red LED turns 
//  10) Bell rings three (?) times
//  11) Green LED starts blinking 
//  12) Rest period finishes
//  13) return to 6



// To do:
//    hardware - add heatsink to voltage regulator (better yet - redesign power supply)
//    consider adding stop function for start button (currently need to change one of the rotator switches to leave round/rest loop)
//      - will probably need to improve debouncing (current delay(10) doesn't really cut it.
//    modify blinking routine to have light phase in and out
//    consider adding some sort of error checking for switch readings (maybe by check for specific ranges or by taking multiple readings. mostly cause of error would be if wire comes loose)


const int runningLED = 13; //green LED (built in LED for now. change to pin 10 for mockup)
const int pauseLED = 9; // red LED
const int solenoidPin = 11;
const int startButtonPin = 12;
const int roundLengthPin = A0;
const int restLengthPin = A1;

int roundLengthSelection = 0; //0 value causes loop to go straight to pause state
int restLengthSelection = 0;

unsigned long roundLength; // in milliseconds
unsigned long restLength; 
unsigned long endOfRound;
unsigned long endOfRest;

Button startButton = Button(startButtonPin,BUTTON_PULLUP_INTERNAL); 

enum states { 
  pause,
  start,
  in_round,
  rest_period
};

states state = pause; // remove?

// the setup routine runs once when you press reset:
void setup() {

  pinMode(solenoidPin, OUTPUT);
  digitalWrite(solenoidPin, LOW);
  
  pinMode(runningLED, OUTPUT);
  digitalWrite(runningLED, LOW);
  
  pinMode(pauseLED, OUTPUT);
  digitalWrite(pauseLED, LOW);
  
  Serial.begin(9600);
  Serial.println("ready");
}

// the loop routine runs over and over again forever:
void loop() {
  
  checkInputs();  // every loop check for changes in button states
  
  switch(state){
    
    case pause: // powering the unit up doesn't go straight to bell - have to push start button to get things going.

      digitalWrite(pauseLED, HIGH); // red LED on
      digitalWrite(runningLED, LOW); // green LED off
      break;    
    
    case start: //read switch settings and then begin round. get here either by hitting start button or when rest period ends

      digitalWrite(pauseLED, LOW); // red LED off
      
      Serial.println("start");
      roundLength = getRoundLengthMillis(roundLengthSelection);
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
      //tone(solenoidPin, 500, 500); // signifies beginning of round // replace with solenoid control - single ding           
      ding(); // single ding to announce beginning of round
      break;

    case in_round:
      
      digitalWrite(runningLED, HIGH); // solid green LED
      if ((long)(millis() - endOfRound) >= 0){ //this approach should cover rollover situation. (see http://playground.arduino.cc/Code/TimingRollover )
        state = rest_period;
        Serial.print("ding ding ding. Round over. Timing error = "); Serial.println((long)(millis() - endOfRound));
        //bell(); // ******* add triple buzz/ding here ******** // signifies end of round
        tripleDing(); // three rings of the bell to announce end of round
      }      
      break;
      
    case rest_period:
      
      blinkRunningLED();
      
      if ((long)(millis() - endOfRest) >= 0){
        state = start;
        Serial.print("DING. Rest over. Timing error = "); Serial.println((long)(millis() - endOfRest));       
      }
      
      break;

  } // end switch
     
} // end loop()

//******************************************************
void checkInputs(){

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
  
  // check start button
  if (startButton.uniquePress()) {
    state = start;
    delay(10); // uniquePress seems a little glitchy (not a very effective debounce). this should cover any bounces
  }  
} // end checkInputs

//******************************************************
int getRoundLengthMillis(int selection){
  
  switch(selection){
    case 0:
      Serial.println("bad reading");
      return(0); //      
    case 1:
      return(5000); //five seconds
    case 2:
      return(10000); // ten seconds
    case 3:
      return(15000);
    case 4:
      return(20000);
    case 5:
      return(25000);
    case 6:
      return(25000); // currently only five settings desired so if six comes up should be treated same as 5
    // implement default as error catcher?
  } // end switch
  
} //end getRoundLengthMillis()

//******************************************************
int getRestLengthMillis(int selection){
  
  switch(selection){
    case 0:
      Serial.println("bad reading");
      return(0); // 
    case 1:
      return(5000); //five seconds
    case 2:
      return(10000); // ten seconds
    case 3:
      return(15000);
    case 4:
      return(20000);
    case 5:
      return(25000);
    
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

//******************************************************
void bell(){ // for testing without solenoid
  tone(solenoidPin, 500, 500);
  delay(700); //tone doesn't block, so delay begins while tone is still playing
  tone(solenoidPin, 500, 500);
  delay(700);
  tone(solenoidPin, 500, 500);
  
}

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
  delay(40);
  digitalWrite(solenoidPin, LOW);
  Serial.println("ding");
  
}

void tripleDing(){

  int i;
  for (i=0; i < 3; i++){
    digitalWrite(solenoidPin, HIGH);
    delay(40);
    digitalWrite(solenoidPin, LOW);
    delay(100);
    Serial.println("ding");
  }
}
