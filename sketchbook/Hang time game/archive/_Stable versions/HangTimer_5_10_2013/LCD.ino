
void setupLCD(){
 //enable LCD Backlight
  pinMode(9, OUTPUT);

  backlightOn();
  
  lcd.begin(16, 2); //16 columns, 2 rows
  lcd.print("Setting up LCD");
  delay(100);
  lcd.clear(); // clear screen and position cursor in upper left
  
} // end setupLCD()

void backlightOn(){
  digitalWrite(9, HIGH);
}

void backlightOff(){
  digitalWrite(9, LOW);
}

void updateLCD(){
  // add code here to only update if values changed. this should fix lcd flicker
  lcd.clear(); 
  lcd.setCursor(0, 0); // column 0, row 0
  lcd.print(lineOne);
  lcd.setCursor(0, 1); // column 0, row 1
  lcd.print(lineTwo);
  
} // end updateLCD()



