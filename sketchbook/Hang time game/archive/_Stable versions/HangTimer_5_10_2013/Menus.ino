void selectUserMenu(char userStr[]){
  //static int i = 0; // replaced with userIndex which is stored in EEPROM

  //userIndex is updated in setupEE()
  constrain(userIndex,0,5); // in case EEPROM returns strange value
  
  if( userIndex>0 && lButton.uniquePress()){
    userIndex--;
    Serial.println("L");
  }
  
  if( userIndex<5 && rButton.uniquePress()){
    userIndex++;
    Serial.println("R");
  }
  
  switch(userIndex){ // in hindsight this would have been better implemented in an array of strings
  case 0:
    strcpy(userStr,"Hanna");
    break;
  case 1:
    strcpy(userStr,"Anna");
    break;
  case 2:
    strcpy(userStr, "Arlin");
    break;
  case 3:
    strcpy(userStr,"Younghee");
    break;
  case 4:
    strcpy(userStr,"Minhee");
    break;
  case 5:
    strcpy(userStr,"Guest");
    break;
  default: // in case i gets out of range somehow
    strcpy(userStr,"error");
    break;
   
 } // end switch(i)   
    
} // end selectUserMenu()

