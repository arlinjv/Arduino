/* known DallasTemperature and OneWire commands:
 *  sensors.isParasitePowerMode()
 *  sensors.getAddress(tempDeviceAddress, i) -- i being number of device
 *  sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
 *  Serial.print(sensors.getResolution(tempDeviceAddress), DEC);
 *  sensors.getTempC(deviceAddress) 
 *  sensors.getTempF(deviceAddress)
 *  DallasTemperature::toFahrenheit(tempC)
 *  sensors.requestTemperatures(); // Send the command to get temperatures (global request to all devices on bus)
 *  
 * 
 */

// function to print a device address
void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

String addressToString(DeviceAddress address){
  String addressString = "";
  
  for (uint8_t i = 0; i < 8; i++){
    if(address[i] < 16) addressString += "0";
    addressString += address[i];
  }
  
  return addressString;
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress) {
  // method 1 - slower
  //Serial.print("Temp C: ");
  //Serial.print(sensors.getTempC(deviceAddress));
  //Serial.print(" Temp F: ");
  //Serial.print(sensors.getTempF(deviceAddress)); // Makes a second call to getTempC and then converts to Fahrenheit

  // method 2 - faster
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
}

void initThermos() {
  // Start up the library

  DeviceAddress tempDeviceAddress;
  
  Serial.println("Setting up Onewire ...");
  
  sensors.begin();
  
  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();
  
  // locate devices on the bus
  Serial.print("Locating devices...");
  
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  
  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices; i++)
  {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i))
  {
    Serial.print("Found device ");
    Serial.print(i, DEC);
    Serial.print(" with address: ");
    printAddress(tempDeviceAddress);
    Serial.println();
    
    Serial.print("Setting resolution to ");
    Serial.println(TEMPERATURE_PRECISION, DEC);
    
    // set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
    sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
    
    Serial.print("Resolution actually set to: ");
    Serial.print(sensors.getResolution(tempDeviceAddress), DEC); 
    Serial.println();
  }else{
    Serial.print("Found ghost device at ");
    Serial.print(i, DEC);
    Serial.print(" but could not detect address. Check power and cabling");
  }
  }
  Serial.println("-------------------------------------------");

}

void updateTemps(){
 
  DeviceAddress tempDeviceAddress;
  
  sensors.requestTemperatures();
  
  for(int i=0;i<numberOfDevices; i++) {
    if(sensors.getAddress(tempDeviceAddress, i)){
      thermos[i].tempC = sensors.getTempC(tempDeviceAddress);
      thermos[i].address = addressToString(tempDeviceAddress); // got error - may be best to store String in address attribute
    }
  }
 
}
/*
 * ++++++++++++++++ Snippets ++++++++++++++++++++++++
 * 
  // call sensors.requestTemperatures() to issue a global temperature request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");  
  
  // Loop through each device, print out temperature data
  for(int i=0;i<numberOfDevices; i++) {
    // for(int i=0;i<numberOfDevices; i++) { Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i)) {
      // Output the device ID
      Serial.print("Temperature for device: ");
      Serial.println(i,DEC);
    
      // It responds almost immediately. Let's print out the data
      printTemperature(tempDeviceAddress); // Use a simple function to print out the data
    }
  //else ghost device! Check your power requirements and cabling
  }

 */

