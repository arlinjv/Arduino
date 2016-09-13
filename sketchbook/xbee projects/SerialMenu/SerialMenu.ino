// Implement serial interface based menu system for xbee based interaction
/*
Menu items:
API - print out all commands that this node recognizes
Help - human friendly description of API and other relevant info
Devices - list devices residing on this node


*/

String inString = " ";
String outString = " ";

void setup(void){
  Serial.begin(9600); // USB connection (standard Arduino serial port) used primarily for testing and monitoring
  Serial1.begin(9600); // XBee
  
  inString.reserve(100);
  outString.reserve(100);

} // end setup
//================================================================

void loop(void){
  
  while (Serial.available()>0) {
    // Serial will be used primarily for testing and monitoring purposes
    outString += (char)Serial.read(); //string to send to remote (coordinator) XBee
  }
  
  if (outString != " "){ //should add flag to determine direction of outstring - menu or xbee
    outString.trim(); // needed to take " " off front. crude but easy. (can't declare an empty string)
    Serial.print("To XBee:");Serial.println(outString);
    Serial1.print(outString);
    outString = " ";
  }
  

  
  while (Serial1.available()>0) { // most commmunications will occur via alternate serial port (connected to XBee)
    inString +=(char)Serial1.read();
    delay(2); // give arduino a moment to catch up. 2 ms seems to be sweet point. no delay and messages get split. longer delays don't seem to help at all
  }
  
  if (inString != " "){ //
    inString.trim();
    Serial.print("From  XBee:"); Serial.println(inString); // send copy of incoming data to serial monitor
    
    checkMenu(inString); // check to see if inString is a menu item
    
    inString = " ";
  }

  delay(100);
} // end loop()
//=======================================================


