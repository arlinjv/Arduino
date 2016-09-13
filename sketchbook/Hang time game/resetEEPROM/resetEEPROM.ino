#include <EEPROM.h>

void setup(){
  
  Serial.begin(115200);
  Serial.println("Resetting all addresses to default (255)");
  
  for (int i = 0; i < 1024; i++) 
    EEPROM.write(i,255);
    
  dumpEEPROM();
    
} // end setup() 


void loop() {
  
  // do nothing
  
} // end loop()


void dumpEEPROM(){
  Serial.println("Dumping EEPROM:   ");
  
  int i = 0;
  
  while(i<=1023){
    
    Serial.print(i); Serial.print(": ");
    
    for (int j = 0; j < 7; j++) {
      if (i>1023) {
        return;
      }
      byte value = EEPROM.read(i);
      Serial.print(value); 
      Serial.print("  ");
      i++;      
      
    } // end for()
    
    Serial.println();
    
  } // end while()
} // end dumpEEPROM()
