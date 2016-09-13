////// Adapted from http://boolscott.wordpress.com/2010/02/04/arduino-processing-analogue-bar-graph-2/ //////

// To do: rewrite printBars to accommodate ping data.

String xLabel = "Readings";
String yLabel = "uS";
String Heading = "raw and smoothed values compared";
String URL = "...";
float Vcc = 5.0; // the measured voltage of your usb <<<<+++++++++++
int NumOfVertDivisions=5; // dark gray
int NumOfVertSubDivisions=10; // light gray
int NumOfBars=2; // you can choose the number of bars, but it can cause issues
                 // since you should change what the arduino sends
                 // if these are changed, background image has problems
                 // a plain background solves the problem
int ScreenWidth = 600, ScreenHeight=400;

/////////////////////////////////////////////////////////
// Serial port stuff ///////////////////////
import processing.serial.*;
Serial myPort;
boolean firstContact = false;
int[] serialInArray = new int[2]; // changed from '6' to '2'. assuming that this is the same value as NumOfBars <<<<<<<<+++++++++++++++
int serialCount = 0;
///////////////////////////////////////////////

int LeftMargin=100;
int RightMArgin=80;
int TextGap=50;
int GraphYposition=80;
float BarPercent = 0.4;
int value;
PFont font;
PImage bg; // not used
int temp;
float yRatio = 0.58;
int BarGap, BarWidth, divisionsWidth;
int[] bars = new int[NumOfBars];

void setup(){
  // bg = loadImage("BG.jpg");
  /// NB SETTINGS ////////////////////////////////////////////////////////
  myPort = new Serial(this, "COM16", 115200);
  ////////////////////////////////////////////////////////////////////////
  divisionsWidth = (ScreenWidth-LeftMargin-RightMArgin)/(NumOfBars);
  BarWidth = int(BarPercent*float(divisionsWidth));
  BarGap = divisionsWidth - BarWidth;
  size(ScreenWidth,ScreenHeight);
  font = createFont("Arial",12);
  textAlign(CENTER);
  textFont(font);
} // end setup()

void draw(){
  // background(bg);   // My one used a background image, I've commented it out and put a plain colour
  background(250);      
  // Headings();       // Displays bar width, Bar gap or any variable.
  Axis();
  Labels();
  PrintBars();
} // end draw()

// Send Receive data //
void serialEvent(Serial myPort) {
  // read a byte from the serial port:
  int inByte = myPort.read();
  if (firstContact == false) {
    if (inByte == 'A') {
      myPort.clear(); // clear the serial port buffer
      firstContact = true; // you've had first contact from the microcontroller
      myPort.write('A'); // ask for more
    }
  }
  else {
  // Add the latest byte from the serial port to array:
    serialInArray[serialCount] = inByte;
    serialCount++;
    // If we have 2 bytes: (changed from 6)
    if (serialCount > 1 ) { // changed to 1 from 5
      for (int x=0;x<2;x++){ // chenged to 2 from 6
        bars[x] = int (yRatio*(ScreenHeight)*(serialInArray[x]/256.0)); // <<<<<+++++ probably need to fiddle with this
      }
    // Send a capital A to request new sensor readings:
    myPort.write('A');
    // Reset serialCount:
    serialCount = 0;
    }
  }
} // end serialEvent

/////// Display any variables for testing here//////////////

void Headings(){
 fill(0 );
 text("BarWidth",50,TextGap );
 text("BarGap",250,TextGap );
 text("divisionsWidth",450,TextGap );
 text(BarWidth,100,TextGap );
 text(BarGap,300,TextGap );
 text(divisionsWidth,520,TextGap );
} // end Headings

void PrintBars(){
  int c=0;
  for (int i=0;i<NumOfBars;i++){
    fill((0xe4+c),(255-bars[i]+c),(0x1a+c));
    stroke(90);
    rect(i*divisionsWidth+LeftMargin, ScreenHeight-GraphYposition, BarWidth, -bars[i]);
    fill(0x2e,0x2a,0x2a);
    text(float(bars[i])/(yRatio*(ScreenHeight))*Vcc, i*divisionsWidth+LeftMargin+BarWidth/2, ScreenHeight-bars[i]-5-GraphYposition );
    text("A", i*divisionsWidth+LeftMargin+BarWidth/2 -5, ScreenHeight-GraphYposition+20 );
    text(i, i*divisionsWidth+LeftMargin+BarWidth/2 +5, ScreenHeight-GraphYposition+20 );
    
    print(bars[i]); print(",");
  } // end for
  println();
} // end PrintBars

void Axis(){
  strokeWeight(1);
  stroke(220);
  for(float x=0;x<=NumOfVertSubDivisions;x++){
    int bars=(ScreenHeight-GraphYposition)-int(yRatio*(ScreenHeight)*(x/NumOfVertSubDivisions));
    line(LeftMargin-15,bars,ScreenWidth-RightMArgin-divisionsWidth+50,bars);
  }
  strokeWeight(1);
  stroke(180);
  for(float x=0;x<=NumOfVertDivisions;x++){
  int bars=(ScreenHeight-GraphYposition)-int(yRatio*(ScreenHeight)*(x/NumOfVertDivisions));
    line(LeftMargin-15,bars,ScreenWidth-RightMArgin-divisionsWidth+50,bars);
  }
  strokeWeight(2);
  stroke(90);
  line(LeftMargin-15, ScreenHeight-GraphYposition+2, ScreenWidth-RightMArgin-divisionsWidth+50, ScreenHeight-GraphYposition+2);
  line(LeftMargin-15,ScreenHeight-GraphYposition+2,LeftMargin-15,GraphYposition);
  strokeWeight(1);
} // end Axis 

void Labels(){
  textFont(font,18);
  fill(50);
  rotate(radians(-90));
  text(yLabel,-ScreenHeight/2,LeftMargin-45);
  textFont(font,16);
  for(float x=0;x<=NumOfVertDivisions;x++){
    int bars=(ScreenHeight-GraphYposition)-int(yRatio*(ScreenHeight)*(x/NumOfVertDivisions));
    text(round(x),-bars,LeftMargin-20);
  }
  textFont(font,18);
  rotate(radians(90));
  text(xLabel,LeftMargin+(ScreenWidth-LeftMargin-RightMArgin-50)/2,ScreenHeight-GraphYposition+40);
  textFont(font,24);
  fill(50);
  text(Heading,LeftMargin+(ScreenWidth-LeftMargin-RightMArgin-50)/2,70);
  textFont(font);
  fill(150);
  text(URL,ScreenWidth-RightMArgin-40,ScreenHeight-15);
  textFont(font);
} // end Labels
