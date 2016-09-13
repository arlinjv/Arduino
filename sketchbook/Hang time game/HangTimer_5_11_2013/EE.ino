void setupEE(){
  EEPROM.setMaxAllowedWrites(20);
  EEPROM.setMemPool(0, EEPROMSizeATmega328);  

  userIndexAddress = EEPROM.getAddress(sizeof(int));
  userIndex = EEPROM.readInt(userIndexAddress);
  //Serial.print("User index "); Serial.print(userIndex); Serial.print(" address: "); Serial.println(userIndexAddress);

  for (int i = 0; i <= namesSize; i++){ //namesSize is size of userNames[] 
    recordTimeAddress[i] = EEPROM.getAddress(sizeof(float));
    //Serial.print("recordTimeAddress");Serial.print(i); Serial.print(" ");Serial.println(recordTimeAddress[i]);
  }  
  
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

void resetRecords(){
  
  for (int i = 0; i <= namesSize; i++){
    EEPROM.updateFloat(recordTimeAddress[i],0);
    lcd.clear();
    lcd.setCursor(0, 0); // column 0, row 0
    lcd.print(i);  
    delay(500);
  }  
  
  lcd.clear();
  lcd.setCursor(0, 0); // column 0, row 0
  lcd.print("Records cleared");  
  delay(1000);
  
} // end resetRecords()
