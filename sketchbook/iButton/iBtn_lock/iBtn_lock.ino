#include <OneWire.h>

OneWire ds(12);

byte addr[8];
const byte theKey[] = {0x01, 0x16, 0x23, 0x48, 0x06, 0x0, 0x0, 0x4F};
String keyStatus="";

const int redLED = 4;
const int greenLED = 3;

void setup(void) {
  Serial.begin(115200);
  
  pinMode(redLED, OUTPUT);
  digitalWrite(redLED, LOW);
  
  pinMode(greenLED, OUTPUT);
  digitalWrite(greenLED, LOW);
  
}

void loop(void) {
  getKeyCode();

  if(keyStatus=="ok"){
      byte i; //
      Serial.print("formatted ");
      for( i = 5; i > 0; i--) {
           Serial.print(":");
           Serial.print(addr[i], HEX);           
      }
      
      Serial.print("raw ");
      for ( i = 0; i < 8; i++){
         Serial.print(":");
         Serial.print(addr[i], HEX);
      }
      
      Serial.println();      
  
  if (memcmp(addr, theKey, 8) == 0){
    Serial.println("That's the right key.");
    digitalWrite(greenLED, HIGH);
  }
  else {
    digitalWrite(redLED, HIGH);
  }
  
  }
  else if (keyStatus!="") { Serial.println(keyStatus);}
 
  delay(1000);
  
  digitalWrite(greenLED, LOW); digitalWrite(redLED, LOW);

}

void getKeyCode(){
  byte present = 0;
  byte data[12];
  keyStatus="";
 
  if ( !ds.search(addr)) {
      ds.reset_search(); // Clear the search state so that it will start from the beginning again
      return;
  }
  
  Serial.println("device found");
  
  if ( OneWire::crc8( addr, 7) != addr[7]) {
      keyStatus="CRC invalid";
      return;
  }
 
  if ( addr[0] != 0x01) {
      keyStatus="not DS1990A";
      return;
  }
  keyStatus="ok";
  ds.reset(); //Returns 1 if a device asserted a presence pulse, 0 otherwise.
}
