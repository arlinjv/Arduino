//Think about storing temperatures as unsigned int - convert to binary before storing in eeprom then convert back
//If you declare a function with a two-word return type (e.g. "unsigned int") the environment will not realize 
//it's a function and will not create a prototype for it. That means you need to provide your own, or place the 
//definition of the function above any calls to it. 
//links:  http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1218921214
//        http://www.openobject.org/opensourceurbanism/Storing_Data


// DS18B20 Temperature chip i/o
//http://www.arduino.cc/playground/Learning/OneWire

int HighByte, LowByte, TReading, SignBit, Whole, Fract;
float Tc_100, Tf_100;

byte present = 0;
byte data[12];
byte addr[8];
OneWire ds(10);  // on pin 10



byte enumerate() {
  byte count = 0; 
  Serial.println("count\taddress");
  while (ds.search(addr)) {
    count++;
    Serial.print(count,DEC);
    Serial.print("\t");
    printAddr();
    Serial.println();
  }
  return count;
}


void getTemps() {
  byte i,j;
  ds.reset_search(); //Begin a new search. The next use of search will begin at the first device. 
  for (i = 0; i < deviceCount; i++) {
    ds.search(addr);  //Search for the next device. The addrArray is an 8 byte array. If a device is found, addrArray is filled with the device's address and true is returned. If no more devices are found, false is returned. 
    Serial.print(i,DEC);
    Serial.print("-");
    printAddr();
    Serial.print(", ");
    
    ds.reset();                 // Returns 1 if a device asserted a presence pulse, 0 otherwise.
    ds.select(addr);            // chooses the device with rom equal to address (0x55)
    ds.write(0x44,1);           // start conversion, with parasite power on at the end
    delay(1000);      // maybe 750ms is enough, maybe not
                    // we might do a ds.depower() here, but the reset will take care of it.
  
    present = ds.reset();
    ds.select(addr);    
    ds.write(0xBE);         // Read Scratchpad
 
  
    for ( j = 0; j < 9; j++) {           // we need 9 bytes
      data[j] = ds.read();
    }
    LowByte = data[0];
    HighByte = data[1];
    TReading = (HighByte << 8) + LowByte; // for processing sketch remember max value here is 7FFh, anything larger is due to (neg) sign bits
    Serial.print("H"); Serial.print(HighByte,HEX); Serial.print(" L");Serial.print(LowByte,HEX);Serial.print("=>");
    Serial.print(TReading);  // lose this and use data[0] and data[1] for eeprom
    Serial.print(" ");
    Serial.print(tempC(TReading));
    Serial.print("C");
    Serial.print("\t");
  }
  Serial.println();
}
 
void printAddr() {
  byte bitCount = 8;
  byte i;
  for (i = 0; i < bitCount; i++) {
    Serial.print(addr[i],HEX);
  }
}

float tempC(int TReading) {
   Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25, the resolution of the 18b20
   return (Tc_100/100);
}

 /*
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
  
*/
