#include <EEPROMEx.h>

float recordTime = 3.3;
float valueRead;
const int memBase = 0;

void setup(){

  Serial.begin(9600);
  
  EEPROM.setMemPool(memBase, EEPROMSizeATmega328);  
  Serial.println("Setting new value at address 0: ");
  
  bool goodWrite = EEPROM.writeFloat(0, recordTime);
  if (goodWrite) Serial.println("successful write");
  valueRead = EEPROM.readFloat(0);
  
  Serial.print("Value read back = "); Serial.println(valueRead);
  
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
