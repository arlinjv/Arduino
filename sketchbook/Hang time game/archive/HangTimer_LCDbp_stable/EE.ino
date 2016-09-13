


void setupEE(){
  
  EEPROM.setMaxAllowedWrites(maxAllowedWrites);
  recordTime = EEPROM.readFloat(recordTimeAddress);
  Serial.print("Record time = "); Serial.println(recordTime);
  
}

