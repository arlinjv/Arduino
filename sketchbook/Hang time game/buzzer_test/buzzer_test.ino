
float hangSeconds = 0; // number of seconds person holds on bar

unsigned long startTime;
unsigned long endTime;

enum states { 
  standby, //implement later. have lights off. maybe low power mode. 
  ready,
  counting,
  done
};

states state = counting;



void setup() { 
  
  Serial.begin(115200);
  
  startTime = millis();
  
  setupBuzzer();
  
}

void loop() {

  updateBuzzer();
  
  endTime = millis();
  hangSeconds = endTime - startTime;
  hangSeconds = hangSeconds/1000;


  
  delay(10);

} // end loop()

