#include <Button.h>


int led = 13; //built in LED
int roundLengthPin = A0;
int restLengthPin = A1;


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
	delay(10); // for debounce
        digitalWrite(led,HIGH);
        Serial.println(readRotatorSwitch(0));
        Serial.println(readRotatorSwitch(1));
  }else{
	digitalWrite(led,LOW);
  }
  
   
}

int readRotatorSwitch(int switchPin){
  //Experimental switch values from six element 740 ohm resistor ladder: 0, 168, 340, 511, 682, 853, 1023 <--- old values
  //change analogVals and valCount to reflect current hardware setup.
  
  int analogVals[] = {0, 194, 392, 589, 786, 984, 1023};
  int valCount = 7; // = length of analogVals. used for iterator below. could use sizeof() but this is easier to read
  int selection = 0; // return value of 0 indicates bad reading
  int switchReading = analogRead(switchPin);
  Serial.print("Switch reading on "); Serial.print(switchPin); Serial.print(": "); Serial.println(switchReading);

  for(int val = 0; val < valCount; val++){
    if (switchReading >= (analogVals[val] - 20)){ //20 represents arbitrary sensitivity setting. on breadboard readings vary no more than +/- 1
      selection++;
    }
  }
  
  return selection;
}


