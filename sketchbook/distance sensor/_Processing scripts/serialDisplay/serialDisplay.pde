// Example by Tom Igoe 
 
import processing.serial.*; 
 
Serial myPort;    // The serial port
PFont myFont;     // The display font
String inString;  // Input string from serial port
int lf = 10;      // ASCII linefeed 
 
void setup() { 
  size(400,200); 
  // You'll need to make this font with the Create Font Tool 
  myFont = loadFont("ArialUnicodeMS-18.vlw"); 
  textFont(myFont, 18); 
  // List all the available serial ports: 
  println(Serial.list()); 
  // I know that the first port in the serial list on my mac 
  // is always my  Keyspan adaptor, so I open Serial.list()[0]. 
  // Open whatever port is the one you're using. 
  myPort = new Serial(this, "COM16", 115200); 
  myPort.bufferUntil(lf); 
} 
 
void draw() { 
  background(0); 
  text("received: " + inString, 10,50); 
} 
 
void serialEvent(Serial p) { 
  inString = p.readString(); 
} 
