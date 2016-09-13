// EEPROM code goes here.
// EEPROM include needed on main tab
// EEPROM capacity is 512 bytes addressed from 0 to 511
// EEPROM.write(address, value)
//    address: the location to write to, from 0 to 511 (int) 
//    value: the value to write, from 0 to 255 (byte)
/*EEPROM memory layout: 
  0:  hour
  1:  minute
  2:  recording interval (minutes)
  3:  device count
  4 - 511:  data
  
*/

// setup EEPROM

const byte HOUR_ADDR = 0;
const byte MINUTE_ADDR = 1;
const byte INTERVAL_ADDR = 2;
const byte DEVICE_COUNT_ADDR = 3;
const byte DATA_START = 4; // good to start on even number so last element not left undefined
const int LAST_DATA_ADDR = 511;
const unsigned long MINUTE_IN_MILLIS = 60000;

unsigned long previousMillis = 0;
int currentAddr;// = DATA_START; - moved to beginLog
boolean fullEE = false;

void beginLog() {
  currentAddr = DATA_START;
  initEE();
  padEE();
  Serial.flush();
  
  delay(startDelay*MINUTE_IN_MILLIS); 
    
  while ( !(Serial.available()) ) { // attach serial reset disable jumper to continue logging after disconnect
    if ( checkInterval() ) {
      logTemps();
    }
    if (fullEE) {
      break;
    }
  }
  Serial.println("logging discontinued");
  Serial.flush();
}
// end beginLog **********************************************************************

void initEE() {
  if (DateTime.available()) {
    EEPROM.write(HOUR_ADDR,byte(DateTime.Hour));
    EEPROM.write(MINUTE_ADDR,byte(DateTime.Minute));
  } else {
    EEPROM.write(HOUR_ADDR,255);  // 255 indicates bad hour or minute
    EEPROM.write(MINUTE_ADDR,255);
  }
  EEPROM.write(INTERVAL_ADDR,interval);
  EEPROM.write(DEVICE_COUNT_ADDR,deviceCount); 
} // end initEE

void padEE() {
  int i;
  for (i = DATA_START; i <= LAST_DATA_ADDR; i++) {
    EEPROM.write(i,0);
  }
  fullEE = false;
}// end padEE

boolean checkInterval() {
  if (millis() - previousMillis > (interval*MINUTE_IN_MILLIS)) {
    previousMillis = millis();
    return true;
  }
  else {
    return false;
  } 
}  // end checkInterval

void logTemps() { //really should have created a seperate temp class, now i have to repeat most of the getTemps code here
//                  write high and low bytes to memory for each reading. called once for each interval
  byte present = 0;
  byte i,j;
  int HighByte = 0;
  int LowByte = 0;
  int TReading;
  byte data[12];
  byte addr[8];
  OneWire ds(10);  // on pin 10
  
  ds.reset_search();
  for (i = 0; i < deviceCount; i++) {
    ds.search(addr);
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
    TReading = (HighByte << 8) + LowByte;    
    
    if (currentAddr <= (LAST_DATA_ADDR - 1)) {
      EEPROM.write(currentAddr,HighByte);
      Serial.print(currentAddr,DEC); Serial.print(" H"); Serial.print(HighByte,HEX);
      currentAddr++;
      EEPROM.write(currentAddr,LowByte);
      Serial.print(currentAddr,DEC);  Serial.print(" L");  Serial.print(LowByte,HEX); Serial.print(" ");
      Serial.print ("=>"); Serial.print(TReading); Serial.print("\t");
      currentAddr++;
    }
    else {
      fullEE = true;
      Serial.print("memory full. stopped at address:");
      Serial.println( (currentAddr-1),DEC);
      break;
    }
  }
  Serial.println();
} // end logTemps*******************************************

void printLog() {
  int i,j;
  int HB, LB, TR;
  Serial.print("logging started at: ");
  Serial.print(EEPROM.read(0),DEC);
  Serial.print(":");
  Serial.print(EEPROM.read(1),DEC);
  Serial.print("\tinterval = ");
  Serial.print(EEPROM.read(2),DEC);
  Serial.print("\tdevice count (EEprom position 3) = ");
  Serial.println(EEPROM.read(3),DEC);
  
  for (i = DATA_START; i <= (LAST_DATA_ADDR - 1); ) {   // i is incremented below
    for (j = 0; j < deviceCount; j++ ) {
      Serial.print(i,DEC); Serial.print("-");
      HB = EEPROM.read(i++);
      Serial.print(HB,HEX);
      Serial.print(" ");
      LB = EEPROM.read(i++);
      Serial.print(LB,HEX);
      TR = (HB << 8) + LB;
      Serial.print(","); Serial.print(tempC(TR));
      Serial.print("\t");
    }
    Serial.println();
  }
  Serial.println();
}  // end printLog ***********************************

void dumpLog() {
  int i;
  for (i = 0; i <= LAST_DATA_ADDR; i++) {
    Serial.print( EEPROM.read(i),BYTE );
  }
  Serial.println();
}

