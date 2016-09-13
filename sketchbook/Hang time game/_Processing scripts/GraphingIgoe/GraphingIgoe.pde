// Graphing sketch based on Tom Igoe's sketches at http://arduino.cc/en/Tutorial/Graph
 
 // This program takes ASCII-encoded strings from the serial port graphs them. It expects values in the
 // range 0 to 1023 (or whatever corresponding max value is , followed by a newline, or newline and carriage return
 

 
import processing.serial.*;
 
Serial myPort;        // The serial port
int xPos = 1;         // horizontal position of the graph
 
void setup () {
 // set the window size:
  size(800, 300);        
  
  // List all the available serial ports
  println(Serial.list());
  // Open whatever port is the one you're using.
  myPort = new Serial(this, "COM16", 115200);
  // don't generate a serialEvent() unless you get a newline character:
  myPort.bufferUntil('\n');
  // set inital background:
  background(0);
} // end setup()

void draw () {
  // everything happens in the serialEvent()
} // end draw()

void serialEvent (Serial myPort) {

  // get the ASCII string:
  String inString = myPort.readStringUntil('\n');
  
  if (inString != null) {
    // trim off any whitespace (/n):
    inString = trim(inString);
    // convert to an int and map to the screen height:
    float inByte = float(inString);
    println(inByte);
    inByte = map(inByte, 0, 1800, 0, height);
 
    // draw the line:
    stroke(127,34,255);
    strokeWeight(2);
    line(xPos, height, xPos, height - inByte);
 
    // at the edge of the screen, go back to the beginning:
    if (xPos >= width) {
      xPos = 0;
      background(0);
    }
   else {
     // increment the horizontal position:
     xPos++;
   }// end else
 } // end if
} // end serialEvent()
