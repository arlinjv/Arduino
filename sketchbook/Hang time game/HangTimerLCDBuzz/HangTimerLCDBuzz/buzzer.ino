// piezo buzzer connected to pin 3. has internal oscillator so only need to connect to power to make sound.
// consider replacing this tab with a buzzer class

int buzzerPin = 3;
int buzzLength = 50; // milliseconds to leave buzzer on
int buzzCount;
// boolean buzzRequest = false;
boolean newSecond = false; // start buzzing after first full second


unsigned long buzzStart;
unsigned long buzzStop = 0;
unsigned long nextBuzz = 0;
int buzzRest = 50;

int currentSecond = 0;

void setupBuzzer (){
  pinMode(buzzerPin, OUTPUT);
}

void buzzerOn(){
  digitalWrite(buzzerPin, HIGH);
}
  
void buzzerOff(){
  digitalWrite(buzzerPin, LOW);
}


void checkNewSecond(){ //change to function returning boolean???
  if (int(hangSeconds) > currentSecond) { //truncate hangSeconds and compare
    currentSecond++;
    newSecond = true;
    Serial.print("Hang seconds = "); Serial.print(hangSeconds);Serial.print("  Current second = "); Serial.println(currentSecond);
  }
} // end checkNewSecond()

void checkNewRecord(){
  if (hangSeconds > recordTime) {
    newRecord = true;
    buzzCount = 10;    
  } 
}  // end checkNewRecord()

// =============================================================
void updateBuzzer(){
  if (state != counting) {
    return;
  }
  
  checkNewSecond();
  
  if (newRecord == false) { // only needs to be true once
    checkNewRecord();
  }
  
  if (newSecond == true) {
    buzzCount += 1; // don't want to cut new record buzzes short
    newSecond = false;
  }
  
 
    
  if (millis() >= nextBuzz){
    if (buzzCount > 0){
      buzzCount--;
      Serial.print("buzzCount = ");Serial.println(buzzCount);
      buzzerOn();
      nextBuzz = millis() + buzzLength + buzzRest;
    }
  }
 
 if ( millis()>= nextBuzz - buzzRest ){ // got a fix this. seems pretty ugly as is. 
   buzzerOff();
   Serial.println("buzzer off");
 }  
  
   
} // end updateBuzzer()


