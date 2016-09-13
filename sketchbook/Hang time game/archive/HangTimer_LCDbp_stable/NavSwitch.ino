
void setupNavSwitch(){
  //This will be ground for the switch
  pinMode(10, OUTPUT);
  digitalWrite(10, LOW); 
  //These are the inputs from the switch
  pinMode(11, INPUT);
  pinMode(12, INPUT);
  pinMode(13, INPUT);
  //Set internal pull-up resistors
  digitalWrite(11, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);
 
} // end setupNavSwitch




/*
  
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
