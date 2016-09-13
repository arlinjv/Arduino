void setupEE(){
  EEPROM.setMaxAllowedWrites(20);
  EEPROM.setMemPool(0, EEPROMSizeATmega328);  

  recordTime = EEPROM.readFloat(recordTimeAddress);
  Serial.print("Record time = "); Serial.println(recordTime);
  
  checkForReset();
  
} // end setupEE()

void checkForReset(){
  delay(500);
  
  if (cButton.uniquePress()){
    lcd.clear();
    lcd.setCursor(0, 0); // column 0, row 0
    lcd.print("Clear records?");
    lcd.setCursor(0, 1); // column 0, row 1
    lcd.print("   <Y   N>");
    
    delay(2000); // give user time to push and hold switch to left
  }
  
  if (lButton.uniquePress()){
    resetRecords();
  }
  
} // end checkForReset()

void resetRecords(){ //plural assumes this will be expanded in future
  
  EEPROM.updateFloat(recordTimeAddress,0);  
  lcd.clear();
  lcd.setCursor(0, 0); // column 0, row 0
  lcd.print("Records cleared");  
  delay(1000);
  
} // end resetRecords()
