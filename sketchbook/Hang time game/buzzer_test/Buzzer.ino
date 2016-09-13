// --- Test Version ---

// piezo buzzer connected to pin 3. has internal oscillator so only need to connect to power to make sound.
// consider replacing this tab with a buzzer class

//int buzzerPin = 3;
int buzzLength = 100; // milliseconds to leave buzzer on
boolean buzzRequest = false;
boolean newSecond = false; // start buzzing after first full second
unsigned long buzzStart;

int currentSecond = 0;

void setupBuzzer (){
  pinMode(A5, OUTPUT);
}

void buzzerOn(){
  digitalWrite(A5, HIGH);
}
  
void buzzerOff(){
  digitalWrite(A5, LOW);
}

void buzz(){ // change later to accept int value for number of buzzes and maybe duration
  buzzRequest = true; // this routine may be unnecessary. may handle all buzz related decisions in this tab. only updateBuzzer() needed in main loop.
}

void checkNewSecond(){
  if (newSecond == true){

    //newSecond = false;
  }
  if (millis()/1000 > currentSecond) {
    currentSecond++;
    newSecond = true;
    Serial.println(currentSecond);
  }

} // end checkNewSound()


void updateBuzzer(){
  if (state != counting) {
    return;
  }
  
  checkNewSecond();
  // checkNewRecord();
  
  if (newSecond == true) {
    buzzRequest = true;
    newSecond = false;
  }
  
  if (buzzRequest == true){
    buzzRequest = false;
    buzzerOn();
    buzzStart = millis();
  }
  
  if (millis() - buzzStart >= buzzLength) { // no harm in turning buzzer off even if it's on
    buzzerOff();
  }
   
} // end updateBuzzer()


  
