void checkMenu(String selection){
  byte i;
  
  if (selection == "test"){
    Serial.println("test command received");
    Serial1.print("test msg received.");
  }
  else if (selection == "api"){
    Serial.println("api command received");
    Serial1.print("the API command has not been implemented yet");
  }
  else if (selection == "help"){
    Serial.println("help command received");
    // Todo: check to see if selection starts with 'help' then call help menu to parse further
    Serial1.print("Currently implemented commands are:");
    Serial1.print("test, api, help, devices, thermo1, thermo2");
  }
  else if (selection == "devices"){
    Serial.println("devices command received");
    //Serial1.print(deviceCount); Serial1.print(" ds devices detected");
    Serial1.print("["); // return list of devices
    for (i = 0; i < deviceCount; i++) {
      Serial1.print("'thermo"); Serial1.print(i+1); Serial1.print("'");
      if (i < (deviceCount -1)) {
        Serial1.print(",");
      }
    }
    Serial1.print("]");
  }
  else if (selection == "thermo1"){ // need to amend this if start making units with more than two sensors
    Serial.println("thermo1 called");
    getTemps();
    Serial1.print("{\"device\":\"thermo1\",\"msgtype\":\"poll\",\"data\":\"");
    Serial1.print(temps[0]);
    Serial1.print("\"}");
  }
  else if (selection == "thermo2"){ // need to amend this if start making units with more than two sensors
    Serial.println("thermo2 called");
    getTemps();
    Serial1.print("{\"device\":\"thermo2\",\"msgtype\":\"poll\",\"data\":\"");
    Serial1.print(temps[1]);
    Serial1.print("\"}");
  }

  
} // end checkMenu() ----------------------------------------
