#include <Button.h>

// Hardware setup: current vision calls for one start/reset button and two rotary switches. Output will be to one or two indicator lamps and some sort of level shifting
// interface to a physical boxing bell.
// Rotator switches will be wired to a resistor ladder.
// Chasssssswnges in round and rest times will be picked up at end of rest period. 
// Pressing start button causes current period (round or rest) to end and new round to start using current switch settings.

int led = 13; //built in LED
int buzzer = 8;
int roundLengthPin = A0;
int restLengthPin = A1;

unsigned long roundLength; // in milliseconds
unsigned long restLength; 
unsigned long endOfRound;
unsigned long endOfRest;

Button button = Button(12,BUTTON_PULLUP_INTERNAL); 

enum states { 
  pause,
  start,
  in_round,
  rest_period
};

states state = pause;

// the setup routine runs once when you press reset:
void setup() {

  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  
  Serial.begin(9600);
  Serial.println("ready");
}

// the loop routine runs over and over again forever:
void loop() {
  
  if (button.uniquePress()) {
    state = start;
    delay(10); // uniquePress seems a little glitchy (not a very effective debounce). this should cover any bounces
  }
  
  switch(state){
    
    case pause: // powering the unit up doesn't go straight to bell - have to push start button to get things going.
      break;    // might want to add more versatile pause capability later.
    
    case start: //read switch settings and then begin round. get here either by hitting start button
      Serial.println("start");
      roundLength = getRoundLengthMillis();
      restLength = getRestLengthMillis(); // arbitrarily two seconds for now
      Serial.print("round length in milliseconds: "); Serial.println(roundLength);
      Serial.print("rest length in milliseconds: "); Serial.println(restLength);
      endOfRound = millis() + roundLength;
      endOfRest = endOfRound + restLength;
      bell(); // putting bell here simplifies code but adds a few seconds to round (if you consider round to begin and first strike of bell)
      state = in_round;
      break;

    case in_round:
      digitalWrite(led, HIGH); // this is the indicator light to show we are fighting not resting
      if ((long)(millis() - endOfRound) >= 0){ //I think this approach should cover rollover situation. Have to wait 50 days to find out for sure, though ...
        state = rest_period;
        Serial.print("ding ding ding. Round over. Error = "); Serial.println((long)(millis() - endOfRound));
        bell();
      }      
      break;
      
    case rest_period:
      digitalWrite(led, LOW);
      if ((long)(millis() - endOfRest) >= 0){
        state = start;
        Serial.print("ding ding ding. Rest over. Error = "); Serial.println((long)(millis() - endOfRest));       
        // no bell() here because bell is rung at end of start case.
      }
      break;
  } // end switch
     
} // end loop()

int getRoundLengthMillis(){
  
  int selection = (readRotatorSwitch(roundLengthPin));
  
  switch(selection){
    case 0:
      Serial.println("bad reading");
      return(60000); // return arbitrary round length of one minute      
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
    default:
      return(60000); //sixty seconds   
  } // end switch
  
} //end getRoundLengthMillis()

int getRestLengthMillis(){
  
  int selection = (readRotatorSwitch(restLengthPin));
  
  switch(selection){
    case 0:
      Serial.println("bad reading");
      return(60000); // return arbitrary round length of one minute      
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
    default:
      return(60000); //sixty seconds   
  } // end switch
  
} //end getRestLengthMillis()

int readRotatorSwitch(int switchPin){
  //Experimental switch values from six element 740 ohm resistor ladder: 0, 168, 340, 511, 682, 853, 1023
  //change analogVals and valCount to reflect current hardware setup.
  //Assumes identical setup for any switches to be read
  
  int analogVals[] = {0, 168, 340, 511, 682, 853, 1023};
  int valCount = 7; // = length of analogVals. used for iterator below. could use sizeof() but this is easier to read
  int selection = 0; // return value of 0 indicates bad reading
  int switchReading = analogRead(switchPin);
  
  for(int val = 0; val < valCount; val++){
    if (switchReading >= (analogVals[val] - 50)){ //50 represents arbitrary sensitivity setting. on breadboard readings vary no more than +/- 1
      selection++;
    }
  }

  Serial.print("analog reading at selection ");  Serial.print(selection);  Serial.print(": ");  Serial.println(switchReading);
  
  return selection;
  
} // end readRotatorSwitch()

void bell(){
  tone(buzzer, 500, 500);
  delay(700); //tone doesn't block, so delay begins while tone is still playing
  tone(buzzer, 500, 500);
  delay(700);
  tone(buzzer, 500, 500);
  
}
