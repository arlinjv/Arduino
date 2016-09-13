

const int runningLED = 13; //green LED (built in LED for now. change to pin 10 for mockup)
const int pauseLED = 9; // red LED

void setup() {
  pinMode(runningLED, OUTPUT);
  digitalWrite(runningLED, LOW);
  
  pinMode(pauseLED, OUTPUT);
  digitalWrite(pauseLED, LOW);
 
}

void loop(){
  delay(2000);
  
  digitalWrite(runningLED, HIGH);
  digitalWrite(pauseLED, HIGH);
}
