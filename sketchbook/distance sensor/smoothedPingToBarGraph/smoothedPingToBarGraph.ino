// intended to work with barGraph.pde adapted from http://boolscott.wordpress.com/2010/02/04/arduino-processing-analogue-bar-graph-2/


// to do:
// - improve smoothing by rejecting out of range values

#include <NewPing.h>

#define TRIGGER_PIN  12  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     11  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input (of length numReadings)
int index = 0;                  // the index of the current reading
unsigned int total = 0;                  // the running total
unsigned int smoothedPing = 0;           // the average

int inByte = 0; // incoming serial byte
int outByteRaw = 0; // need to map to 0 to 255 for export to processing
int outByteSmoothed = 0;

int maxDistUs = 1500;

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

void setup() {
  Serial.begin(9600); // Open serial monitor at 115200 baud to see ping results.
  
  establishContact(); // send a byte to establish contact until Processing responds 

  
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0; 
}

void loop() {
  delay(50);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  
  // if we get a valid byte, read analog ins:
 if (Serial.available() > 0) {
   // get incoming byte:
   inByte = Serial.read();
    unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
  
    if (uS > maxDistUs) // 1500 gives ~ 10 inches. also higher values lead to integer overflow(wraparound?) problem
      uS = maxDistUs;

    total= total - readings[index];       // subtract the last reading
    readings[index] = uS;                 // add the current reading value
    total= total + readings[index];       // add the reading to the total
    index = index + 1;                    // advance to the next position in the array:  

    // if we're at the end of the array...
    if (index >= numReadings)              
      // ...wrap around to the beginning: 
      index = 0;                           

    // calculate the average:
    smoothedPing = total / numReadings; //smoothedPing is the running average of the values in the readings array

    outByteRaw = map(uS, 0, maxDistUs, 0, 255);
    outByteSmoothed = map(smoothedPing, 0, maxDistUs, 0, 255);
  
    delay(100); //for debugging. remove once problems resolved
    Serial.write(outByteRaw);
    delay(100);
    Serial.write(outByteSmoothed);
 } // end if (serial.available...
 
} // end loop()

void establishContact() {
  while (Serial.available() <= 0) {
  Serial.write('A'); // send a capital A
  delay(300);
  }
} // end establishContact()
