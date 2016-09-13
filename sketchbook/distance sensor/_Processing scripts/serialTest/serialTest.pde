

int ScreenWidth = 600, ScreenHeight=400;

/////////////////////////////////////////////////////////
// Serial port stuff ///////////////////////
import processing.serial.*;
Serial myPort;
boolean firstContact = false;

///////////////////////////////////////////////

int inByte;

void setup(){
  
  myPort = new Serial(this, "COM16", 9600);
  
  size(ScreenWidth,ScreenHeight);

  
} // end setup()

void draw(){

  background(250);      
  // show the plot area as a white box

  if (myPort.available() > 0) {
    inByte = myPort.read();
    if (inByte == 'A') {
      println("'A' received. Bytes remaining - " + myPort.available());
      myPort.clear();
    } // end if
    else {
      println ("inByte = " + inByte + ". Bytes remaining - " + myPort.available());
    } // end else

  } // end if
  
} // end draw()

void keyReleased() {
  if (key == ENTER) {
    println();
  } // end if
  else {
    myPort.write('A');
  } // end else
  
} // end keyPressed()






