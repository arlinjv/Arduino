// to do - implement low battery signal


const int ledPin = 11;
const int pirPin = 10;

void setup(){
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  pinMode(pirPin, INPUT);
  
  Serial.begin(9600);
}


void loop(){

  delay(500);
  if (digitalRead(pirPin) == HIGH) {
    Serial.println("Motion sensed");
  }
  
} // end loop()
