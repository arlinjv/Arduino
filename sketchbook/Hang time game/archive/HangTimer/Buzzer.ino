// buzzerPin defined in main tab
unsigned long buzzStart;
unsigned long buzzStop = 0;
unsigned long nextBuzz = 0;
const int buzzLength = 50; // milliseconds to leave buzzer on
const int buzzRest = 50;


void updateBuzzer(){
    
  if (millis() >= nextBuzz){ // this will always be true first time through
    if (buzzCount > 0){
      buzzCount--;
      //Serial.print("buzzCount = ");Serial.println(buzzCount);
      buzzerOn();
      nextBuzz = millis() + buzzLength + buzzRest;
    }
  }
 
 if ( millis()>= nextBuzz - buzzRest ){ // gotta fix this. seems pretty ugly as is. 
   buzzerOff();
   //Serial.println("buzzer off");
 }
 
} // end updateBuzzer()


void setupBuzzer (){
  pinMode(buzzerPin, OUTPUT);
}

void buzzerOn(){
  digitalWrite(buzzerPin, HIGH);
}
  
void buzzerOff(){
  digitalWrite(buzzerPin, LOW);
}

