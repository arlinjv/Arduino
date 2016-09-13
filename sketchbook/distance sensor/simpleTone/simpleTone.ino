#include <Button.h>

Button button = Button(13,BUTTON_PULLUP_INTERNAL); 

enum states { 
  on,
  off
};

states state = off;

void setup() {
  Serial.begin(9600);
}

void loop() {
  if (button.uniquePress()){
    switch(state){
      
      case on:
        state = off;
        noTone(8);
        break;
        
      case off:
        state = on;
        tone(8,441);
        break;
        
    } // end switch
  } // end if
} // end loop
