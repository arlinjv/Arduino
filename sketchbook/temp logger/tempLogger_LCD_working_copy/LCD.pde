//using Sparkfun serial lcd, partno LCD-00461. Appears to be discontinued.
//Reference:
//    http://www.sparkfun.com/datasheets/LCD/SerLCD_V2_5.PDF (pdf saved in program directory
//    http://www.arduino.cc/playground/Learning/SparkFunSerLCD


int txPin = 2;

SoftwareSerial LCD = SoftwareSerial(0, txPin);
// since the LCD does not send data back to the Arduino, we should only define the txPin

void setupLCD () {
  pinMode(txPin, OUTPUT);
  LCD.begin(9600);
  clearLCD();
  LCD.print("LCD setup");
  delay(500);
  clearLCD();
}

void clearLCD(){
   LCD.print(0xFE, BYTE);   //command flag
   LCD.print(0x01, BYTE);   //clear command.
}

void selectLineOne(){  //puts the cursor at line 0 char 0.
   LCD.print(0xFE, BYTE);   //command flag
   LCD.print(128, BYTE);    //position
}
void selectLineTwo(){  //puts the cursor at line 1 char 0.
   LCD.print(0xFE, BYTE);   //command flag
   LCD.print(192, BYTE);    //position
}
