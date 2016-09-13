const int LED = 13;
const int buttonPin = 12;

long time;
long newTime;
long interval;

void setup(){

  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  Serial.begin(9600);
  
  time = millis();
  
}

void loop(){

    digitalWrite(LED, !digitalRead (LED));
    delay(1000);    
  
}
