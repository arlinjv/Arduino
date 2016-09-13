//using Sparkfun serial lcd, partno LCD-00461. Appears to be discontinued.
//Reference:
//    http://www.sparkfun.com/datasheets/LCD/SerLCD_V2_5.PDF (pdf saved in program directory
//    http://www.arduino.cc/playground/Learning/SparkFunSerLCD
//    https://www.sparkfun.com/tutorials/246 (sample scripts and chart showing addresses for screen positions on 2 line 16 char display)


// initialize serial communication with LCD:
int txPin = 2;
SoftwareSerial LCD = SoftwareSerial(0, txPin); //wanted this to be in LCD tab, but compiler stated LCD below was out of scope.

void setupLCD () {
  pinMode(txPin, OUTPUT);
  LCD.begin(9600);
  clearLCD();
  LCD.print("LCD setup");
  delay(500);
  clearLCD();
}

void clearLCD(){
   LCD.write(0xFE);   //command flag
   LCD.write(0x01);   //clear command.
}

void selectLineOne(){  //puts the cursor at line 0 char 0.
   LCD.write(0xFE);   //command flag
   LCD.write(128);    //position
}
void selectLineTwo(){  //puts the cursor at line 1 char 0.
   LCD.write(0xFE);   //command flag
   LCD.write(192);    //position
}

void updateLCD() {
  char digits[10];
  dtostrf(hangSeconds, 5, 1, digits); // see http://arduino.cc/forum/index.php?topic=85523.0 for discussion of dtostrf. needed because sprintf can't handle floats in arduino
  
  if (state == done) {
    if (newRecord) {
      lineOne = "New Record!!!";
    }
    else {
      lineOne = "Done.....";
    }
  }
  
  selectLineOne();
  LCD.print(lineOne);
  selectLineTwo();
  LCD.print(digits);
} // end updateLCD() 
