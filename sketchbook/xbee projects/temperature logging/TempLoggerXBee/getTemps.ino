int HighByte, LowByte, TReading, SignBit, Whole, Fract;
float Tc_100, Tf_100;

byte present = 0;
byte data[12];
byte addr[8];


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


void getTemps() { // obtains readings from each device and stores in temps[]
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
    temps[i] = tempC(TReading);
    Serial.print(temps[i]);
    Serial.print("C");
    Serial.print("\t");
   
  } // end for loop (deviceCount)
  Serial.println();
} // end getTemps() ---------------------------------------------
 
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

