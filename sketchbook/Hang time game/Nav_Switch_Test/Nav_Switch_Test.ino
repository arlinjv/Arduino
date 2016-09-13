#include <Button.h>

// uses sparkfun nav switch - https://www.sparkfun.com/products/8236

const int nSwitchLeftPin = 13;
const int nSwitchCenterPin = 12;
const int nSwitchRightPin = 11;
const int nSwitchCommon = 10;

Button lButton = Button(nSwitchLeftPin,BUTTON_PULLUP_INTERNAL);
Button rButton = Button(nSwitchRightPin,BUTTON_PULLUP_INTERNAL);
Button cButton = Button(nSwitchCenterPin,BUTTON_PULLUP_INTERNAL);

void setup(){
  
  Serial.begin(115200);
  
  setupNavSwitch();

  
} // end setup()

void loop(){
  
  delay(20);
  
  if(lButton.uniquePress()) {
    Serial.println("left");
  }

  if(rButton.uniquePress()) {
    Serial.println("right");
  }
  
  if(cButton.uniquePress()) {
    Serial.println("center");
  }
  
} // end loop()
