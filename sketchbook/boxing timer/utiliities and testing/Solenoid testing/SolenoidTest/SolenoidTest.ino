
const int solenoidPin = 10;
const int ledPin = 13;
int inByte = 0;

//int potReading = 0;
int pulseLength = 0;
int pulseInterval = 100;

void setup(){
  Serial.begin(115200);
  Serial.println("Ready");
  
  pinMode(solenoidPin, OUTPUT);  
  digitalWrite(solenoidPin, LOW);
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
}

void loop(){
  delay(100);    
  
  if (Serial.available() > 0) {
    inByte = Serial.read();
    if (inByte == 'a') {
      pulse();      
    }
    else if (inByte == 'i'){
      pulseInterval = Serial.parseInt();
      Serial.print("interval = "); Serial.println(pulseInterval);
    }
    else if (inByte == 'l'){
      pulseLength = Serial.parseInt();
      Serial.print("pulse length = "); Serial.println(pulseLength);
    }
    else if (inByte == 'd') {
      Serial.print("pulse length = ");Serial.print(pulseLength);Serial.print("    interval = "); Serial.println(pulseInterval);
    }
  }
}// end loop()



//***********************************************************************
void pulse(){
  Serial.print("pulse out - "); Serial.println(pulseLength);
  digitalWrite(solenoidPin, HIGH);
  digitalWrite(ledPin, HIGH);
  delay(pulseLength);
  digitalWrite(solenoidPin, LOW);
  digitalWrite(ledPin, LOW);
  delay(pulseInterval);
} // end pulse()
