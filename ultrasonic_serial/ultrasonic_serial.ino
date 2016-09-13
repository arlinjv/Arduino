#include <medianFilter.h> //https://github.com/jeroendoggen/Arduino-signal-filtering-library

/*
*
*Based on: http://www.14core.com/wiring-esp8266-nodemcu-with-hcsr04-ultrasonic-sensor/
*
*To do:
* - Implement serial menu
* - take readings only when prompted
* - implement filtering (average multiple readings and throw out invalid readings.)
*
*/

const int TRIGGER = D1;
const int ECHO = D2;

String inString = " ";
String outString = " ";

medianFilter Filter;
long filteredDistance;
long distance;

const int MAX_DISTANCE = 5000; // 5300 centimeters seems to be the 'default' value when the sensor times out.
                               // maximum pulse from HC-SR04 appears to be about 290 milliseconds
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

    if (++readings > 5){ // will keep trying for 1.5 seconds (if trying five times)
      dist = -1; // prevent blocking due to bad device or no obstacles within range.
    }
    
    delay(1);    
  } while (dist > MAX_DISTANCE);
  
  return dist;
}



void setup() {
  
  Serial.begin (115200);
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);

  inString.reserve(100);
  outString.reserve(100);  

  Serial.println("setup complete");
}

void loop() {
  
  while (Serial.available()>0) { // most commmunications will occur via alternate serial port (connected to XBee)
    inString +=(char)Serial.read();
    delay(2); // give arduino a moment to catch up. 2 ms seems to be sweet point. no delay and messages get split. longer delays don't seem to help at all
  }
  
  if (inString != " "){ //
    inString.trim();
    //Serial.println(inString); // send copy of incoming command to serial monitor
    //checkMenu(inString); // check to see if inString is a menu item

    if (inString == ""){
      distance = getDistance();
      filteredDistance = Filter.run(distance);
      Serial.print(" Raw:\t\t"); Serial.print(distance); Serial.print("\tFiltered:\t\t"); Serial.println(filteredDistance);      
    }
    
    inString = " ";
  }

  delay(100);
  

}

