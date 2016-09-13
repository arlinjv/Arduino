#include <EEPROMEx.h>



void setup(){

  Serial.begin(115200);
  
  EEPROM.setMemPool(0, EEPROMSizeATmega328);  
  
  dumpEEPROM(); 
}


void loop(){
  // do nothing
}


void dumpEEPROM(){
  Serial.println("Dumping EEPROM:   ");
  
  int i = 0;
  
  while(i<=1023){
    
    Serial.print(i); Serial.print(": ");
    
    for (int j = 0; j < 7; j++) {
      if (i>1023) {
        return;
      }
      byte value = EEPROM.readByte(i);
      Serial.print(value); 
      Serial.print("  ");
      i++;      
      
    } // end for()
    
    Serial.println();
    
  } // end while()
} // end dumpEEPROM()
