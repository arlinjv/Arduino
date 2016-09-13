#include <OneWire.h>

// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!
// http://milesburton.com/Dallas_Temperature_Control_Library

OneWire  ds(10);  // on pin 10 (a 4.7K resistor is necessary)

void setup(void) {
  
  Serial.begin(9600);
  Serial1.begin(9600); // XBee
  
  digitalWrite(11, HIGH); // to power sensors
  
} // end setup()

void loop(void) {
  
  if (Serial.available()>0) {
    /// read the incoming byte:
    int incomingByte = Serial.read();
    if (incomingByte == 'A'){
      sendReadings();
    }
    else if (incomingByte == 'h'){
      Serial1.print("send an A to receive readings");
    } 
  }
  
  if (Serial1.available()>0) { // for leonardo board
    /// read the incoming byte:
    int incomingByte = (char)Serial1.read();
    if (incomingByte >= 0){
      Serial.print(incomingByte);
    }
    if (incomingByte == 'A'){
      sendReadings();
    }
    else if (incomingByte == 'h'){
      Serial1.print("send an A to receive readings");
    }
  }
  
  delay(100);
  
} // end loop()

void sendReadings(){
  String transmitString;
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;
  int loops = 0;
  
  // old example output (too long, need to keep under 80 bytes): [{"device_id":"\x28\x5c\x38\x02\x02\x00\x00\xbf","device_type":"thermometer_F","data":"64.5"},{"device_id":"\x28\xdc\x2f\x02\x02\x00\x00\x78","device_type":"thermometer_F","data":"65.2"}]
  // example output [{"id":"\x28\x5c\x38\x02\x02\x00\x00\xbf","data":"64.5"}, {.....}]
  transmitString = "["; // we are creating a json array of objects (python dicts)
  
  while (ds.search(addr)){
    //loop through all devices here
    if (loops > 0) {
      transmitString += ","; // needed to separate each additional object
    }
    
    loops += 1;
    
    transmitString += "{";
    //transmitString += "ROM ="; //Serial.print("ROM =");
    
    transmitString += "\"id\":\""; // "id":"
    for( i = 0; i < 8; i++) {
      //transmitString += "\\x"; //Serial.print('\\'); Serial.print('x');
      if (addr[i] < 0x10) {transmitString += "0";}; // add leading zero - python won't accept one digit hex val
      transmitString += String(addr[i], HEX);//Serial.print(addr[i], HEX);
    }
    transmitString += "\","; //closing quotation mark
    
    //transmitString += ",\"device_type\":\"thermometer_F\","; // consider using chip id code below to identify device type
    
      // the first ROM byte indicates which chip
    switch (addr[0]) {
      case 0x10:
        //Serial.println("  Chip = DS18S20");  // or old DS1820
        type_s = 1;
        break;
      case 0x28:
        //Serial.println("  Chip = DS18B20");
        type_s = 0;
        break;
      case 0x22:
        //Serial.println("  Chip = DS1822");
        type_s = 0;
        break;
      default:
        //Serial.println("Device is not a DS18x20 family device.");
        return;
    } // end switch() 

    ds.reset(); //Reset the 1-wire bus. Usually this is needed before communicating with any device. 
    ds.select(addr); //Select a device based on its address. After a reset, this is needed to choose which device you will use, and then all communication will be with that device, until another reset. 
    ds.write(0x44, 1);        // start conversion, with parasite power on at the end
  
    delay(1000);     // maybe 750ms is enough, maybe not
    // we might do a ds.depower() here, but the reset will take care of it.
  
    present = ds.reset();
    ds.select(addr);    
    ds.write(0xBE);         // Read Scratchpad

    //Serial.print("  Data = ");  //Serial.print(present, HEX);  //Serial.print(" ");
    for ( i = 0; i < 9; i++) {           // we need 9 bytes
      data[i] = ds.read();
      //Serial.print(data[i], HEX);
      //Serial.print(" ");
    }
  
    // Deleted CRC check code

    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    int16_t raw = (data[1] << 8) | data[0];
    if (type_s) {
      raw = raw << 3; // 9 bit resolution default
      if (data[7] == 0x10) {
        // "count remain" gives full 12 bit resolution
        raw = (raw & 0xFFF0) + 12 - data[6];
      }
    } else {
      byte cfg = (data[4] & 0x60);
      // at lower res, the low bits are undefined, so let's zero them
      if (cfg == 0x00) raw = raw & ~7;  // 9 bit resolution, 93.75 ms
      else if (cfg == 0x20) raw = raw & ~3; // 10 bit res, 187.5 ms
      else if (cfg == 0x40) raw = raw & ~1; // 11 bit res, 375 ms
      //// default is 12 bit resolution, 750 ms conversion time
    }
    celsius = (float)raw / 16.0;
    fahrenheit = celsius * 1.8 + 32.0;
    //Serial.print("  Temperature = ");  //Serial.print(celsius);  //Serial.print(" Celsius, ");
    //transmitString += ("F"); //Serial.print("F");
    
    transmitString += "\"data\":";
    
    transmitString += "\"";
    char digits[6];
    dtostrf(fahrenheit, 5, 1, digits);
    transmitString += digits; //Serial.print(fahrenheit);
    transmitString += "\"";
    //Serial.println(" Fahrenheit");

    transmitString += "}";
    
  } // end while()
  
  transmitString += "]";
  Serial.println(transmitString);
  Serial1.print(transmitString);  
  //Serial.println(); // no more addresses 
  ds.reset_search(); // begin a new search. The next use of search will begin at the first device
  delay(250);
  return;
  
} // end sendReadings()
