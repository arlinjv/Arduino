void superMenu() { // determine if manual or auto mode, called at beginning of setup
  while (!Serial.available() ) {
    //Serial.print('.'); this was nice for creating prompt for manual users but makes processing program too difficult
    //delay(500);
} // loop until receive input while printing periods
  char selection = Serial.read();
  if (selection == 'a') {
    dumpLog();
    Serial.flush();
    superMenu();  // 
  }
} // end superMenu
//******************************************

boolean menu() {
  Serial.println("-----Enter s to set time, m to set interval(minutes), d to display system info (time, interval, temperatures ...,");
  Serial.println("     D to set start delay, t to test, L to begin logging, O to output EEPROM");
  while (!Serial.available()){
  }
  char selection = Serial.read();
  Serial.println(selection);
  if (selection == 's') {
    if ( setTimeManual() )  {
      return true;
    }
  } 
  else if (selection == 'd') {   // display system info
    digitalClockDisplay();
    Serial.print("interval is set to ");
    Serial.print(interval);
    Serial.print(" minutes.  Start delay is ");
    Serial.print(startDelay);
    Serial.println(" minutes.");
    getTemps();
    return true;
  } 
  else if (selection == 'D') { // set start delay
    if ( setStartDelay(startDelay) ) { //startDelay passed by reference here
      return true;
    }
  }
  else if (selection == 'm') {
    if ( setInterval(interval) ) {
      return true;
    }
  }
  else if (selection == 'L') {
    beginLog();
    return true;
  }
  else if (selection == 'O') {
    printLog();
    return true;
  }
  else if (selection == 't') {  // test option
    getTemps();
    logTemps();
    return true;
  }  
  return false;
}// end menu()
//*********************************************************************

void digitalClockDisplay(){
  // digital clock display of current date and time
  DateTime.available(); // must do .sync() first, otherwise returns zero (?)
  Serial.print(DateTime.Hour,DEC);
  printDigits(DateTime.Minute);
  printDigits(DateTime.Second);
  Serial.print(" ");
  Serial.print(DateTimeStrings.dayStr(DateTime.DayofWeek));
  Serial.print(" ");
  Serial.print(DateTimeStrings.monthStr(DateTime.Month));
  Serial.print(" ");
  Serial.println(DateTime.Day,DEC);
} // end digitalClockDisplay
void printDigits(byte digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits,DEC);
}// end printDigits()
//***********************************************************************************


boolean setTimeManual() {
  long hour;
  long minutes;
  Serial.println("Enter time - hhmm");
  hour = getDigit();
  hour = hour*10 + getDigit();
  if (!(hour >= 0 && hour <= 23)) {
    return false;
  } 
  minutes = getDigit();
  minutes = minutes*10 + getDigit();
  if (!(minutes >=0 && minutes<=59)) {
    return false;
  }
  Serial.print(hour); 
  printDigits(minutes);
  Serial.println();

  unsigned long newTime = (946681200 + 3600L) + minutes*60L + hour*3600L; 
  DateTime.sync(newTime);
  return true;
}    // End setTimeManual
//**************************************************************************************

boolean setInterval(unsigned int &_interval) {  // manual prototype added in main tab see discussion here: http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1227572792
  unsigned int temp = 0;
  Serial.println("Enter the logging interval in minutes - xxx (pad front with zeroes)");
  temp = getDigit();
  temp = temp*10 + getDigit();
  temp = temp*10 + getDigit();
  _interval = temp;
  Serial.print("Logging interval set to ");
  Serial.print(_interval);
  Serial.println(" minute(s)");
  return true;
} //end SetInterval()

boolean setStartDelay(unsigned int &_startDelay) {
  unsigned int temp = 0;
  Serial.println("Enter start delay in minutes - xxx (pad front with zeroes)");
  temp = getDigit();
  temp = temp*10 + getDigit();
  temp = temp*10 + getDigit();
  _startDelay = temp;
  Serial.print("start delay set to ");
  Serial.print(_startDelay);
  Serial.println(" minute(s)");
  return true;
} // end setStartDelay
  
  
int getDigit() {
  int i;
  while (!Serial.available()){
  }
  char c= Serial.read();
  if (c>= '0' && c<= '9') {
    i = c - '0';
  }
  return i;
}// end getDigit()
