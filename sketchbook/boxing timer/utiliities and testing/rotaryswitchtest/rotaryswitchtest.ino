#include <Button.h>


int led = 13; //built in LED
int buzzer = 8;
int roundLengthPin = A0;
int restLengthPin = A1;
//int restTimeSelectPin = ;


Button button = Button(12,BUTTON_PULLUP_INTERNAL);


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

  if(button.uniquePress()){ //uniquePress seems a little glitchy. consider using a different technique
	digitalWrite(led,HIGH);
        Serial.print("round length selection: "); Serial.println(readRotatorSwitch(roundLengthPin));
        Serial.print("rest length selection: "); Serial.println(readRotatorSwitch(restLengthPin));
  }else{
	digitalWrite(led,LOW);
  }
  
   
}

int readRotatorSwitch(int switchPin){
  //Experimental switch values from six element 740 ohm resistor ladder: 0, 196, 392, 590, 787, 983, // old: 0, 168, 340, 511, 682, 853, 1023
  //change analogVals and valCount to reflect current hardware setup.
  
  int analogVals[] = {0, 196, 392, 590, 787, 983};
  int valCount = 6; // = length of analogVals. used for iterator below. could use sizeof() but this is easier to read
  int selection = 0; // return value of 0 indicates bad reading
  int switchReading = analogRead(switchPin);

  Serial.println(switchReading);
  for(int val = 0; val < valCount; val++){
    if (switchReading >= (analogVals[val] - 50)){ //50 represents arbitrary sensitivity setting. on breadboard readings vary no more than +/- 1
      selection++;
    }
  }
  
  return selection;
}

void bell(){
  tone(buzzer, 500, 500);
  delay(700); //tone doesn't block, so delay begins while tone is still playing
  tone(buzzer, 500, 500);
  delay(700);
  tone(buzzer, 500, 500);
  
}
