#include <DateTimeStrings.h>
#include <DateTime.h>
#include <EEPROM.h>

/*
*Temperature datalogging sketch
*
*Pseudocode: 
*Print menu
*wait for input
*  prompt for hour
*  prompt for minute
*  prompt for interval
*write time to first two EEPROM slots
*write interval to #3
*log temperatures until full
*  while waiting for input
*    if input says to break
*      discontinue logging 
*Wait for prompt to dump data

progress6/11/09 - implemented function that gets hour and minute
progress 6/12/09 - use hour and minute to set time and then display that time
next steps: prompt user for measurement intervals
            store time and intervals to EEPROM
            fill EEPROM with filler data
            dump EEPROM data on prompt
*/


void setup() {
  Serial.begin(9600);
  time_t arduinoTime = (946681200 + 3600); // first number supposed to be 0000 hours year 2000, yet apparently is 2300 prev. night
  DateTime.sync(arduinoTime); // available() returns false until sync done
  digitalClockDisplay();        
 
}    // End Setup

unsigned int interval = 1;
boolean setInterval(unsigned int &_interval); // manual prototype added here so I can pass interval by reference later on

void loop() {

   
  if (!menu()) {                 // fancy way of calling menu
    Serial.println("try again"); // this code will not be called if valid selection entered
    Serial.flush();
    delay(100);
  }
  
}   // End Loop
//***************************************************************************************

boolean menu() {
  Serial.println("Enter s to set time, m to set interval(minutes), d to display system info (time, interval ...)");
  while (!Serial.available()){}
  char selection = Serial.read();
  Serial.println(selection);
  if (selection == 's') {
    if ( setTimeManual() )  {
      return true;
    }
  } else if (selection == 'd') {
      digitalClockDisplay();
      Serial.print("interval is set to ");
      Serial.print(interval);
      Serial.println(" minutes");
      return true;
  } else if (selection == 'm') {
      if ( setInterval(interval) ) {
        return true;
      }
  }  
  return false;
}

void digitalClockDisplay(){
  // digital clock display of current date and time
  DateTime.available(); 
  Serial.print(DateTime.Hour,DEC);
  printDigits(DateTime.Minute);
  printDigits(DateTime.Second);
  Serial.print(" ");
  Serial.print(DateTimeStrings.dayStr(DateTime.DayofWeek));
  Serial.print(" ");
  Serial.print(DateTimeStrings.monthStr(DateTime.Month));
  Serial.print(" ");
  Serial.println(DateTime.Day,DEC);
}
void printDigits(byte digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits,DEC);
}


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
}

boolean setInterval(unsigned int &_interval) {
  Serial.println("Enter the logging interval in minutes - xxx (max 240 - pad front with zeroes)");
  _interval = getDigit();
  Serial.print("Logging interval set to ");
  Serial.print(_interval);
  Serial.println(" minute(s)");
  return true;
}

int getDigit() {
  int i;
  while (!Serial.available()){}
  char c= Serial.read();
  if (c>= '0' && c<= '9') {
    i = c - '0';
  }
  return i;
}
