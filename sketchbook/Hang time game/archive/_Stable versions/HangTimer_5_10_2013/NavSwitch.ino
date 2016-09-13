// uses sparkfun nav switch - https://www.sparkfun.com/products/8236

void setupNavSwitch(){
  //This will be ground for the switch
  pinMode(nSwitchCommon, OUTPUT);
  digitalWrite(nSwitchCommon, LOW); 
  
  /* this code not needed with button library
  //These are the inputs from the switch
  pinMode(nSwitchRightPin, INPUT);
  pinMode(nSwitchCenterPin, INPUT);
  pinMode(nSwitchLeftPin, INPUT);
  //Set internal pull-up resistors
  digitalWrite(nSwitchRightPin, HIGH);
  digitalWrite(nSwitchCenterPin, HIGH);
  digitalWrite(nSwitchLeftPin, HIGH);
  */
   
} // end setupNavSwitch




/*
// example code for using button library:

  if(lButton.uniquePress()) {
    Serial.println("left");
  }


  
// from Nav_Switch example sketch
  if(digitalRead(13)==LOW){
  lcd.setCursor(0, 1);
  lcd.print("Left     ");
  }
  if(digitalRead(12)==LOW){
  lcd.setCursor(0, 1);
  lcd.print("Click    ");
  }
  if(digitalRead(11)==LOW){
  lcd.setCursor(0, 1);
  lcd.print("Right    ");
  }
  
*/
