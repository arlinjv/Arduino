// https://learn.adafruit.com/using-ifttt-with-adafruit-io/arduino-code-1
// http://www.esp8266-projects.com/2015/03/internal-adc-esp8266.html

// R1 = 1.03Mohm red red yellow
// R2 = 217Kohm (brown black green)
// Vout = V at adc, Vin = battery voltage
// Vout = Vin * R2/(R1+R2) where Vin --> R1 --> Vout --> R2 --> GND
// 217Kohm/(217Kohm+1030Kohm) = 0.174 (= 87/500)
// Vout = Vin * 0.174
// Vout = adc/1023
// Vin = Vout/0.174
// adc/1023 = Vout/0.174
// Vout = adc/1023*0.174 = adc/178.02 = adc*0.0056179144 = adc*0.00562

int batteryLevel() {

  // read the battery level from the ESP8266 analog in pin.
  // analog read level is 10 bit 0-1023 (0V-1V).
  // our 1M & 220K voltage divider takes the max
  // lipo value of 4.2V and drops it to 0.758V max.
  // this means our min analog read value should be 580 (3.14V)
  // and the max analog read value should be 774 (4.2V).
  int level = analogRead(A0);

  // convert battery level to percent
  //level = map(level, 580, 774, 0, 100);
  Serial.print("Battery level: "); Serial.print(level); Serial.println("%");
  return level;

}

float vBat() {

  int adc = analogRead(A0);

  float volts = adc*0.00562;

  Serial.print("adc: "); Serial.print(adc);Serial.print("\tV battery: "); Serial.println(volts);
  return(volts);

}

char * vBatStr() {

  delay(20); // thinking it might help to rest in case recent transmissions (high draw) affect reading

  int adc = analogRead(A0);
  float volts = adc*0.00562;

  Serial.print("adc: "); Serial.print(adc);Serial.print("\tV battery: "); Serial.println(volts);
  
  static char bVstr [5];
  dtostrf(volts,5, 2, bVstr);

  return(bVstr);
}

char * vBatJson() {

  delay(20); // thinking it might help to rest in case recent transmissions (high draw) affect reading

  int adc = analogRead(A0);
  float volts = adc*0.00562;

  Serial.print("adc: "); Serial.print(adc);Serial.print("\tV battery: "); Serial.println(volts);

  // sample response: {"adc":"123","Vbat":"3.90","percent":"--"}
  StaticJsonBuffer<100> jsonBuffer; // using calculator at https://rawgit.com/bblanchon/ArduinoJson/master/scripts/buffer-size-calculator.html gives 56 bytes
  JsonObject& root = jsonBuffer.createObject();
  root["adc"] = adc;
  root["volts"]= volts; // or maybe: double_with_n_digits(volts,5);;
  root["percent"] = "--";

  char buffer[100];
  root.printTo(buffer,sizeof(buffer));
  
  return(buffer);
}

