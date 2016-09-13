#include <OneWire.h>

// DS18B20 Temperature chip i/o

int HighByte, LowByte, TReading, SignBit, Whole, Fract;
int j;
float Tc_100, Tf_100;
byte i;
byte present = 0;
byte data[12];
byte addr[8];
OneWire ds(10);  // on pin 10


void setup(void) {
  // initialize inputs/outputs
  // start serial port
  Serial.begin(9600);
  Serial.println("setup");
}

void loop(void) {

 
  if ( !ds.search(addr)) {  //returns a 1 if next device has been enumerated. 0 if no more or some error
       ds.reset_search();   //reset_search required to start another search from the beginning
       return;
  }
 
 
  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.print("CRC is not valid!\n");
      return;
  }
 
  if ( addr[0] != 0x28) {
      Serial.print("Device is not a DS18S20 family device.\n");
      return;
  }
 
 // Tell device to start measuring temp
  ds.reset();                 // Returns 1 if a device asserted a presence pulse, 0 otherwise.
  ds.select(addr);            // chooses the device with rom equal to address (0x55)
  ds.write(0x44,1);           // start conversion, with parasite power on at the end
 
  delay(1000);      // maybe 750ms is enough, maybe not
                    // we might do a ds.depower() here, but the reset will take care of it.
 
 //get result of temp conversion
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad
 
  
    for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
  }
  
  LowByte = data[0];
  HighByte = data[1];
  TReading = (HighByte << 8) + LowByte;
  SignBit = TReading & 0x8000;  // test most sig bit
  if (SignBit) // negative
  {
    TReading = (TReading ^ 0xffff) + 1; // 2's comp
  }
  Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25, the resolution of the 18b20
 
  // now convert to F by multiplying C by 1.8 and adding 32
  Tf_100 = Tc_100*9/5 + 3200;
  
 
  if (SignBit) // If its negative
  {
     Serial.print("-");
  }
  
//  Serial.print("C=");
//  Serial.print(Tc_100/100);

  Serial.print("address = ");
  for (j = 0; j < 8; j++) {
    Serial.print(int(addr[j]));
  }
  Serial.print(" F=");
  Serial.print(Tf_100/100);

  Serial.print("\n");
  delay(2000);
}


