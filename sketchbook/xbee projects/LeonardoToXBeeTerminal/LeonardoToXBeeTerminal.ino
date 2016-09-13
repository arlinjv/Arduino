String inString = " ";
String outString = " ";

void setup(void){
  Serial.begin(9600); // USB connection
  Serial1.begin(9600); // XBee
  
  inString.reserve(100);
  outString.reserve(100);

}

void loop(void){
  
  while (Serial.available()>0) {
    outString += (char)Serial.read(); //string to send to remote XBee
  }
  
  if (outString != " "){
    outString.trim(); // needed to take " " off front. crude but easy. (can't declare an empty string)
    Serial.print("To XBee:");Serial.println(outString);
    Serial1.print(outString);
    outString = " ";
  }
  

  
  while (Serial1.available()>0) {
    inString +=(char)Serial1.read();
    delay(2); // give arduino a moment to catch up. 2 ms seems to be sweet point. no delay and messages get split. longer delays don't seem to help at all
  }
  
  if (inString != " "){
    inString.trim();
    Serial.print("From  XBee:"); Serial.println(inString);
    inString = " ";
  }

  delay(100);
}
