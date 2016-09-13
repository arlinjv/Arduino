void checkMenu(String selection){
  
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
    Serial1.print("test, api, help, devices");
  }
  else if (selection == "devices"){
    Serial.println("devices command received");
    //listDevices();
  }
  
} // end checkMenu()
