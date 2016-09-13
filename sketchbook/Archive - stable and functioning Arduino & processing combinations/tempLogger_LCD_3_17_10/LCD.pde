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
  LCD.print("test");
}


