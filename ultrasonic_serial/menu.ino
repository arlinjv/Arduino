void checkMenu(String selection){

  if (selection == "r") {
    
  }
  else if (selection == "test"){
    Serial.println("test command received");
    Serial1.print("test msg received.");
  }
  else if (selection == "help"){
    Serial.println("help command received");
    // Todo: check to see if selection starts with 'help' then call help menu to parse further
  }

  
} // end checkMenu()
