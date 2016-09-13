// Serial LCD sample program 
//http://www.nkcelectronics.com/serial-16x2-lcd-bo162.html
// NKC Electronics 
void setup() 
{ 
Serial.begin(9600); 
// Display ON 
Serial.print(0xfe, BYTE); 
Serial.print(0x41, BYTE); 
// Backlight ON 
Serial.print(0xfe, BYTE); 
Serial.print(0x52, BYTE); 
// Home Cursor 
Serial.print(0xfe, BYTE); 
Serial.print(0x46, BYTE); 
// Send some text 
Serial.print("NKC Electronics"); 
// Cursor to column 1, line 2 
Serial.print(0xfe, BYTE); 
Serial.print(0x45, BYTE); 
Serial.print(0x01, BYTE); 
Serial.print(0x02, BYTE); 
// Send some more text 
Serial.print("SerialLCD module"); 
} 
void loop() 
{ 
} 
 
