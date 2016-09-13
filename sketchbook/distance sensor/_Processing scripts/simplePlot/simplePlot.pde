////// Adapted partially from http://boolscott.wordpress.com/2010/02/04/arduino-processing-analogue-bar-graph-2/ //////
/// Adapted largely from LogReader app (Arduino temperature logger project)


int ScreenWidth = 600, ScreenHeight=400;

/////////////////////////////////////////////////////////
// Serial port stuff ///////////////////////
import processing.serial.*;
Serial myPort;
boolean firstContact = false;
int[] serialInArray = new int[2]; // declare empty array of length 2
int serialCount = 0;
///////////////////////////////////////////////

float plotX1, plotY1; // corners of plot box
float plotX2, plotY2;

PFont font;

int[] yVals = new int[2];

int numValuesToPlot = 100; //for now just display first one hundred values.
int plotIndex = 0; // x value for currently displayed data points

void setup(){
  
  myPort = new Serial(this, "COM16", 115200);
  
  size(ScreenWidth,ScreenHeight);
 
  plotX1 = 50; //shifted over to make room for buttons (added 100)
  plotX2 = width - plotX1;
  plotY1 = 60;
  plotY2 = height - plotY1;
  
  font = createFont("Arial",12);
  textFont(font);
  
} // end setup()

void draw(){

  background(250);      
  // show the plot area as a white box
  fill(255);
  rectMode(CORNERS); //instructs rect to take xy values of opposite corners as parameters
  noStroke();
  rect(plotX1, plotY1, plotX2, plotY2);
  
  strokeWeight(5); //this may be superfluous - see drawDataPoints
  stroke(#5679C1); // ditto - but need to make sure some stroke() command is listed in each routine to counter noStroke() above

  drawDataPoints();
} // end draw()

void drawDataPoints() {
  float y = map(yVals[0], 0, 255, plotY2, plotY1);
  float x = map(plotIndex, 0, numValuesToPlot, plotX1, plotX2);
  point (x,y);
  
}// end drawDataPoints

// Send Receive data //
void serialEvent(Serial myPort) {
  // read a byte from the serial port:
  int inByte = myPort.read();
  print(inByte + ", "); // for debugging
  if (firstContact == false) {
    if (inByte == 'A') {
      myPort.clear(); // clear the serial port buffer
      firstContact = true; // we've had first contact from the microcontroller
      myPort.write('A'); // ask for more
    }
  }
  else {
  // Add the latest byte from the serial port to array:
    serialInArray[serialCount] = inByte; //sonar values are mapped to 0-255 range
    serialCount++; //global variable
    // If we have 2 bytes: (two bytes are expected at a time from current arduino sketch)
    if (serialCount > 1 ) { 
      for (int i=0;i<2;i++){ 
        yVals[i] = serialInArray[i]; // value will be mapped to size of display area later
      }
    // Send a capital A to request new sensor readings:
    myPort.write('A');
    // Reset serialCount:
    serialCount = 0;
    println(); // for debugging
    }
  }
} // end serialEvent




