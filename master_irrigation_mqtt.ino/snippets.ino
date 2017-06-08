/*void handleRoot() {
  // make an html page here:
  // show status of sensors
  // add reset button

  String lastReading(msg);// convert character array to String
  String message = "";
  message += "uptime                     - " + getTimeString(millis()) + "\n";
  message += "last reading               - " + msgString + " centimeters";
  
  server.send(200, "text/plain", message);
  Serial.println("device root accessed");

}
*/

/*

long getDistance(){

  long duration, dist;
  int readings = 0;

  do {
    digitalWrite(TRIGGER, LOW);  
    delayMicroseconds(2); 
    
    digitalWrite(TRIGGER, HIGH);
    delayMicroseconds(10); 
    
    digitalWrite(TRIGGER, LOW);
    duration = pulseIn(ECHO, HIGH); // consider adding timeout (third parameter, unsigned long (millis or micros?)
    dist = (duration/2) / 29.1; 

    if (++readings > 4){ // will keep trying for 1.5 seconds (if trying five times)
      dist = -1; // prevent blocking due to bad device or no obstacles within range.
    }
    
    delay(1);    
  } while (dist > MAX_DISTANCE);
  
  return dist;
}


// Alternate method of handling byte array payloads by Mark Rose
class StringWrapper {
  private:
    const char *p;
    int length;

  public:
    StringWrapper(const char *s, int n) : p(s), length(n) {} //http://stackoverflow.com/questions/1272680/what-does-a-colon-following-a-c-constructor-name-do

    int equals(const char *other);

    void print(Stream &out) {
      for (int i=0; i < length; ++i) {
        out.print(p[i]);
      }
    }
    
};

int StringWrapper::equals(const char *other) {
  int n = strlen(other);
  return n==length &&  strncmp(p, other, n)==0;
}

// StringWrapper payloadString(payload, length);
// if (payloadString.equals("ok")) {}
// payloadString.print(Serial);



*/
