/*
void selectUserMenu(char userStr[]){
  //userIndex is updated in setupEE()
 
  constrain(userIndex,0,namesSize); // in case EEPROM returns strange value
  
  if( userIndex>0 && lButton.uniquePress()){
    userIndex--;
    Serial.println("L");
  }
  
  if( userIndex<namesSize && rButton.uniquePress()){
    userIndex++;
    Serial.println("R");
  }
  
  strcpy(userStr,userNames[userIndex]);
  
    
} // end selectUserMenu()
*/
