int led = 13; //built in LED
int buzzer = 8;
int roundLengthPin = A0;
int restLengthPin = A1;

int analogReading;


void setup() {


  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
    
  Serial.begin(9600);
  Serial.println("ready");
}

void loop(){
  
  delay(1000);
  analogReading = analogRead(A0);
  Serial.print("A0 = "); Serial.print(analogReading);
  analogReading = analogRead(A1);
  Serial.print("   A1 = "); Serial.print(analogReading);
  analogReading = analogRead(A2);
  Serial.print("   A2 = "); Serial.print(analogReading);
  analogReading = analogRead(A3);
  Serial.print("   A3 = "); Serial.println(analogReading);

}
