#include <OneWire.h>

// based on http://tushev.org/articles/arduino/item/50-reading-ibutton-with-arduino


OneWire ds(12);

byte addr[8];

String keyStatus="";

void setup(void) {
  Serial.begin(115200);

}

void loop(void) {
  getKeyCode();

  if(keyStatus=="ok"){
      byte i; //
      Serial.print("formatted ");
      for( i = 5; i > 0; i--) { // I think maybe 5 should be changed to six (see below) though it doens't matter for the buttons I have on hand.
           Serial.print(":");
           Serial.print(addr[i], HEX);           
      }
      
      Serial.print(" raw "); //the first byte is the iButton family code, the next 6 are the serial number, and the last is the CRC
      for ( i = 0; i < 8; i++){
         Serial.print(":");
         Serial.print(addr[i], HEX);
      }
      
      Serial.println();      
      
  }
  else if (keyStatus!="") { Serial.println(keyStatus);}
 
  delay(1000);

}

void getKeyCode(){
  byte present = 0;
  byte data[12];
  keyStatus="";
 
  if ( !ds.search(addr)) {
      Serial.println("None or no more.");
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
