#include <Button.h>

//  Hardware setup:  
//  - Two rotary switches  connected to resistor ladders
//  - One red LED and one green LED
//  - solenoid 
//  - pushbutton switch
//  - jumper from pin 2 to ground enables test mode
//
// Objective is to test proper connection and function of all external components (switches, lights, solenoid)
// Functionality:
//  - start button activates solenoid
//  - lights stay on
//  - changes in rotator switch settings are output by serial


const int runningLED = 13; //green LED (built in LED for now. change to pin 10 for mockup)
const int pauseLED = 12; // red LED
const int solenoidPin = 10;
const int startButtonPin = 11;
const int roundLengthPin = A0;
const int restLengthPin = A1;
const int testModePin = 2;

int roundLengthSelection = 0; //0 value causes loop to go straight to pause state
int restLengthSelection = 0;

unsigned long roundLength; // in milliseconds
unsigned long restLength; 
unsigned long endOfRound;
unsigned long endOfRest;

boolean testMode = false;

Button startButton = Button(startButtonPin,BUTTON_PULLUP_INTERNAL); 


void setup() {

  // initialize the digital pin as an output.
  pinMode(runningLED, OUTPUT);
  digitalWrite(runningLED, HIGH); // for testing purposes lights can just stay on 
  
  pinMode(pauseLED, OUTPUT);
  digitalWrite(pauseLED, HIGH); // ditto
  
  pinMode(solenoidPin, OUTPUT);
  digitalWrite(solenoidPin, LOW);
  
  pinMode(testModePin, INPUT);
  digitalWrite(testModePin, HIGH); // turn on internal pullup resistor

  
  Serial.begin(9600);
  Serial.println("ready");
}

// the loop routine runs over and over again forever:
void loop() {
  
  checkInputs();  // every loop check for changes in button states
  delay(50);
     
} // end loop()


//******************************************************
void checkInputs(){

  // test mode enabled by connecting digital pin 2 to ground
  boolean oldTestModeState = testMode;  
  if (digitalRead(testModePin == LOW)) {
    testMode = true;
    digitalWrite(13, !digitalRead(13));
    delay(500);
    digitalWrite(13, !digitalRead(13));
  }
  else {
    testMode = false;
  }  
  if (oldTestModeState != testMode) {
    Serial.println("test mode toggled");
  }

  
  int oldRoundLengthSelection = roundLengthSelection;
  int newRoundLengthSelection = readRotatorSwitch(roundLengthPin);
  
  if (oldRoundLengthSelection != newRoundLengthSelection) {
    roundLengthSelection = newRoundLengthSelection;
    Serial.print("New round length selection = "); Serial.println(roundLengthSelection);

  }
  
  int oldRestLengthSelection = restLengthSelection;
  int newRestLengthSelection = readRotatorSwitch(restLengthPin);
  
  if (oldRestLengthSelection != newRestLengthSelection) {
    restLengthSelection = newRestLengthSelection;
    Serial.print("New rest length selection = "); Serial.println(restLengthSelection);

  }
  
  if (startButton.uniquePress()) {
    Serial.println("start button pressed");
    tripleDing();    
  }  
} // end checkInputs


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
