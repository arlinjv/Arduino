


void setupEE(){
  EEPROM.setMaxAllowedWrites(20);
  
  recordTimeAddress = EEPROM.getAddress(sizeof(float)); 
  recordTime = EEPROM.readFloat(recordTimeAddress);
  Serial.print("Record time = "); Serial.println(recordTime);
  
  userIndexAddress = EEPROM.getAddress(sizeof(int));
  userIndex = EEPROM.readInt(userIndexAddress);
  Serial.print("User index"); Serial.println(userIndex);
  
}

