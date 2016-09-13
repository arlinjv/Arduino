long getSmoothedDistance(){

  const int numReadings = 4;

  int readings[numReadings];      // the readings from the analog input (of length numReadings)
  unsigned int total = 0;                  // the running total

  for (int i = 0; i < numReadings; i++){
    
    do { // keep getting readings until valid reading received
      readings[i] = getDistance();
      Serial.print(readings[i]); Serial.print("   ");
    } while (readings[i] > MAX_DISTANCE);
    
    total += readings[i];
  }
    Serial.print("-->  ");
    return total/numReadings;
}
