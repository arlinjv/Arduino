#include <Button.h>

const int LED = 13;
const int buttonPin = 12;

Button startButton = Button(buttonPin,BUTTON_PULLUP_INTERNAL); 

enum states {
  pause,
  blinking
};

states state = pause;


void setup(){

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  Serial.begin(9600);
  
}

void loop(){

  if (startButton.uniquePress()) {
    Serial.println("button pressed");
    if (state == pause) {
      state = blinking;
    } 
    else 
    {
      state = pause;
    }
    
    delay(50); // for additional debouncing
  } 
  
  switch(state){
   
    case pause:
      digitalWrite(LED, LOW);
      break;
     
    case blinking:
      blinkLED();
      break;
   
  }
  
}

void blinkLED(){
  
  const int blinkTime = 250;
  static unsigned long nextToggleTime; // start value doesn't matter since we will toggle anyway
  
  if ((long)(millis() - nextToggleTime) >= 0){
    digitalWrite(LED, !digitalRead (LED)); 
    nextToggleTime = millis() + blinkTime;
    Serial.println("Blink");
  }
}
