#include <Button.h>

//  Hardware setup:  Two rotary switches  connected to resistor ladders
// Objective is to implement new funcionality of returning to pause state on any change of switch status. 


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

  // initialize the digital pin as an output.
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

      break;    
    
    case start: //read switch settings and then begin round. get here either by hitting start button or end of 

      digitalWrite(pauseLED, LOW); // red LED off
      
      Serial.println("start");
      roundLength = getRoundLengthMillis(roundLengthSelection);
      if (roundLength == 0){
        state = pause;
        break;
      }
      
      restLength = getRestLengthMillis(restLengthSelection); 
      if (restLength == 0){
        state = pause;
        break;
      }
      
      Serial.print("round length in milliseconds: "); Serial.println(roundLength);
      Serial.print("rest length in milliseconds: "); Serial.println(restLength);
      endOfRound = millis() + roundLength;
      endOfRest = endOfRound + restLength;

      state = in_round;

      break;

    case in_round:
      digitalWrite(runningLED, HIGH); // solid green LED
      if ((long)(millis() - endOfRound) >= 0){ //I think this approach should cover rollover situation. Have to wait 50 days to find out for sure, though ...
        state = rest_period;
        Serial.print("ding ding ding. Round over. Timing error = "); Serial.println((long)(millis() - endOfRound));
        bell(); // ******* add triple buzz/ding here ******** // signifies end of round
      }      
      break;
      
    case rest_period:
      //Serial.print("R");
      digitalWrite(runningLED, LOW); // change this to blinking (green LED)
      if ((long)(millis() - endOfRest) >= 0){
        state = start;
        Serial.println("DING. Rest over. Timing error = "); Serial.println((long)(millis() - endOfRest));       

        tone(solenoidPin, 500, 500); // signifies beginning of round // replace with solenoid control - single ding     
      }
      
      break;

  } // end switch
     
} // end loop()

//******************************************************
void checkInputs(){

  int oldRoundLengthSelection = roundLengthSelection;
  int newRoundLengthSelection = readRotatorSwitch(roundLengthPin);
  
  if (oldRoundLengthSelection != newRoundLengthSelection) {
    roundLengthSelection = newRoundLengthSelection;
    Serial.print("New round length selection = "); Serial.println(roundLengthSelection);
    state = pause;
  }
  
  int oldRestLengthSelection = restLengthSelection;
  int newRestLengthSelection = readRotatorSwitch(restLengthPin);
  
  if (oldRestLengthSelection != newRestLengthSelection) {
    restLengthSelection = newRestLengthSelection;
    Serial.print("New rest length selection = "); Serial.println(restLengthSelection);
    state = pause;
  }
  
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
  //Experimental switch values from six element 740 ohm resistor ladder: 0, 168, 340, 511, 682, 853, 1023
  //change analogVals and valCount to reflect current hardware setup.
  //Assumes identical setup for any switches to be read
  
  int analogVals[] = {0, 196, 392, 590, 787, 983};
  int valCount = 6; // = length of analogVals. used for iterator below. could use sizeof() but this is easier to read
  int selection = 0; // return value of 0 indicates bad reading
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
void bell(){
  tone(solenoidPin, 500, 500);
  delay(700); //tone doesn't block, so delay begins while tone is still playing
  tone(solenoidPin, 500, 500);
  delay(700);
  tone(solenoidPin, 500, 500);
  
}
