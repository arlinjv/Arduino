const int sensorPin = A7;


void checkADC(){
  static int lastADC = 95;
  static long noChangeCount = 0;
  int adc = analogRead(sensorPin);

  if (abs(adc-lastADC) < 5){
    noChangeCount++;
  }
  else {
    lastADC = adc;
    Serial.print("adc: "); Serial.print(adc); Serial.print("\tNo change count: "); Serial.println(noChangeCount);
    noChangeCount = 0;
  }
    
}

void setup() {
  pinMode(sensorPin, INPUT);
  Serial.begin(115200);
}

void loop() {

  Serial.println(analogRead(sensorPin));
  
  //checkADC();
  delay(500);
  // put your main code here, to run repeatedly:

}
