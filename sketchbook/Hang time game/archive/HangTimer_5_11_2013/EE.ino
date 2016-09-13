void setupEE(){
  EEPROM.setMaxAllowedWrites(20);
  
  checkForReset();
  recordTimeAddress = EEPROM.getAddress(sizeof(float)); 
  recordTime = EEPROM.readFloat(recordTimeAddress);
  Serial.print("Record time = "); Serial.println(recordTime);
  
  userIndexAddress = EEPROM.getAddress(sizeof(int));
  userIndex = EEPROM.readInt(userIndexAddress);
  Serial.print("User index"); Serial.println(userIndex);
  
} // end setupEE()

void checkForReset(){
  delay(500);
  
  if (cButton.uniquePress()){
    lcd.clear();
    lcd.setCursor(0, 0); // column 0, row 0
    lcd.print("Clear records?");
    lcd.setCursor(0, 1); // column 0, row 1
    lcd.print("   <Y   N>");
    
    delay(2000); // give user time to push switch to left
  }
  
  if (lButton.uniquePress()){
    resetRecords();
  }
  
} // end checkForReset()

void resetRecords(){
  EEPROM.updateFloat(recordTimeAddress,0);
  lcd.clear();
  lcd.setCursor(0, 0); // column 0, row 0
  lcd.print("Records cleared");  
  delay(1000);
} // end resetRecords()
