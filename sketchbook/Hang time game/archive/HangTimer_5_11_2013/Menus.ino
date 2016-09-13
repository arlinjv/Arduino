void selectUserMenu(char userStr[]){
  //userIndex is updated in setupEE()

  char* userNames[] = {"Hanna","Anna","Arlin","Younghee","Minhee","Guest"};
  const int namesSize = 5; // size of array above. be sure to change if adding extra names (didn't want to mess around with sizeof() )
  
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


/*
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
*/
