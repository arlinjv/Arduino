#include <OneWire.h>

/*
To do:
  - implement more generalized thermo routine. (i.e. check to see if inString contains 'thermo' then  check for following digit and respond accordingly

*/

OneWire  ds(10);  // on pin 10 (a 4.7K resistor is necessary)


String inString = " ";
String outString = " ";

byte deviceCount;

float* temps; // will instantiate array below once number of devices is known

void setup(void){
  Serial.begin(9600); // USB connection (standard Arduino serial port) used primarily for testing and monitoring
  Serial1.begin(9600); // XBee
  
  inString.reserve(100);
  outString.reserve(100);
  
  digitalWrite(11, HIGH); // to power sensors
  
  delay(1000); // give time to stabilize
  deviceCount = enumerate();
  Serial.print(deviceCount); Serial.println(" ds devices detected");

  temps = new float[deviceCount];
  
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


