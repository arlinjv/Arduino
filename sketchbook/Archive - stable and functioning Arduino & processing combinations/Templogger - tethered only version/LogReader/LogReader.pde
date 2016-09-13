/*  interface with Arduino program TempLogger. 
    TempLogger waits for 'a' then dumps content of EEPROM
    
    progress:
      7/7/09: sketch now reads log dump straight into a 512 byte array. 
              Yeeha!!! successfully verified correct exchange by printing array as hex values.
              added code to write byte array to file 
      7/10/09: improved byteReader code to provide number of bytes actually received
      7/11/09: demonstrated that this code >>>(highByte << 8) + lowByte;<<< works (not quite. see 8/14/09)
      7/11/09: moved code from keyPressed block in draw to keyReleased() event. 
      7/14/09: created rawTemps array. 
      7/14/09: updated protocol to include value for number of devices (address 3.) Data now starts at address 4
      8/14/09: fixed bug with implementing device count (if number sent by arduino is 0, program falls into infinite loop.)
      8/14/09: fixed bug with byte values from arduino greater than 128 being treated as neg. numbers. used bit masks to fix.
      8/15/09: began optimizing for graphic output. saved backup copy as LogReader_bak
                - created 2D thermos array and packed with data from rawTemps
                - created print routine to print out Celsius values in columns according to device number
      8/19/09: formatted window and created drawDataPoints routine.
      8/30/09: added -- dataMax = max(rawTemps) -- to serialEvent routine - this gives max value across devices
      8/30/09: added code to draw() to plot data for all devices
      8/30/09: began drawTimeLabels routine
      9/1/09: abandoned drawTimeLabels and incorporated into drawDataPoints - makes for inefficient code as time labels get drawn repeatedly each time a device's
              data is displayed. Could do a drawTimeLabels by replicating for (i = ....) loop in drawDataPoints along with float x = map(...).
      9/8/09: overwrote old LogReader_bak file.
      9/8/09: redid drawTimeLabels 
      9/8/09: added drawTempLabelsC
      9/8/09: added rollover highlight code to drawDataPoints - gives C value only and doesn't handle overlapping data points well
      9/16/09: added drawTempsButton.
      9/16/09: added drawTempLabelsC and fixed rollover code in drawDataPoints to give F value when appropriate
      9/17/09: added drawConnectButton and connect routine
      9/17/09: removed (commented out) keyReleased routine - will use connect button now
      9/17/09: changed drawConnectButton - now shows reset after successful connection
      
      
      
    To do:
      - add status text in blank data area (i.e. 'no device', 'connecting' ...)
      - write twoPlaceFloat routine that will return string with any number of digits to left and only two digits to right of decimal. use for labels and rollover text
      - add labels indicating number and color of device being plotted
      - change color of data points to reflect color of label
      - refine rollover code in drawDataPoints - offset each label specific amount and change color of text to match color of datapoint - or move text to box at bottom of screen
      - save data to file so that program can show results from previous sessions
      - figure out bug where single key press results in no results and subsequent presses result in bad data.
        adding delay in keyPressed block doesn't seem to help. (7/11/09) implementing keyReleased() event helps with initial bad data. Now if first key press
        is unsuccessful second press usually results in good data. subsequent presses from there always result in bad data. may be a problem with arduino code.
      - figure out bug where third reset attempt leads to index out of bounds error. using myPort.clear() and setting thermos to null doesn't help.
      - fix connect button to handle instances where there is no connection
      - test with number of devices other than two. not entirely confident in thermos initialization.
      - rewrite using OOP - for instance put button code into a class
        
*/
import processing.serial.*;

Serial myPort;

float plotX1, plotY1;
float plotX2, plotY2;


int dataMin, dataMax;
int startMin, startHour, startTimeSecs;
int interval, intervalSecs;
int numDevices; // third element in arduino data dump

int[][] thermos;

boolean dataReceived = false;

static final int F = -1; //constant indicating temperature scale in use
static final int C = 1;
int tempScale = C; //set default temperature scale to Celsius
int lastClick = 0; // used in button code to prevent rapid fire clicks

float buttonBaseX = 60;
float buttonBaseY = 100;
int buttonWidth = 85;
int buttonHeight = 30;


PFont plotFont;

void setup() {
  size(820, 505);
  
  dataMin = 0;  // assumes C temp scale. leaving dataMin equal to zero reflects a decision to have the bottom value on the scale be zero (i.e. freezing)
                //remember that raw data array is packed with min_int. take this into account if not using arbitrary floor. empty slots could cause big skew in plot
  dataMax = 1600; //equates to 100 deg C. here only as safeguard. dataMax is inititialized properly in serialEvent code
  
  plotX1 = 150; //shifted over to make room for buttons (added 100)
  plotX2 = width - plotX1 + 100; // added 100 to match above
  plotY1 = 60;
  plotY2 = height - plotY1;
  
  plotFont = createFont("SansSerif", 20);
  textFont(plotFont);
  
  smooth();

  myPort = new Serial(this, "COM8", 9600);
}


void draw() {
  background(224);
  
  // show the plot area as a white box
  fill(255);
  rectMode(CORNERS);
  noStroke();
  rect(plotX1, plotY1, plotX2, plotY2);
  
  strokeWeight(5); //this may be superfluous - see drawDataPoints
  stroke(#5679C1); // ditto - but need to make sure some stroke() command is listed in each routine to counter noStroke() above
  
  drawConnectButton(); // disappears after data received. reappears after reset button hit.
  drawTempButton();
  if (dataReceived) {
    for (int i = 0; i < numDevices; i++) {
      drawDataPoints(thermos[i]);
    }
    drawTimeLabels(thermos[0].length, 8); // 8 columns
    if (tempScale == C) {
      drawTempLabelsC();
    } else if (tempScale == F) {
      drawTempLabelsF();
    }
  }
  
}


/*void keyReleased() { // no longer needed - will use connect button
  println("keypress detected");
  myPort.buffer(512);  //serialEvent gets called only when buffer has 512 bytes
  myPort.write(97);// sends 'a' to arduino
  println("start character sent");
} // end keyReleased
// ***************************************************************************
*/

void connect() { // called by drawConnectButton
  myPort.buffer(512); // serialEvent will be called when buffer contains 512 bytes
  myPort.write(97); // sends 'a' to arduino
  println("start character sent");
} // end connect
// ***************************************************************************
  
void serialEvent(Serial myPort) {  // receive data bytes from arduino, convert to raw values and pack into arrays
  byte[] inBuffer = new byte[512];
  int[] rawTemps = new int[254]; // ( 511 (last data address) - 3 (last non data address) ) / 2 
  int tempsIndex = 0;
  
  println("serialEvent called");
  
  while (myPort.available() > 0) {  // this might be part of the repeat case bug. think about changing this to if () 
    int byteCount = myPort.readBytes(inBuffer); // load buffer into inBuffer and return number of bytes received.
    println(byteCount + " bytes received");
    
    if (byteCount > 0) {
      printData(inBuffer);
      
      // pack raw values into rawTemps index  -- remove this once 2D array approach implemented
      for (int i = 4; i <= 510; ) {  // i is incremented twice below
        int highByte = inBuffer[i++];  //bytes must be read in as integers due to two's complement numbering in Java
        int lowByte = inBuffer[i++];
        rawTemps[tempsIndex++] = ((highByte << 8) & 0x0000ffff) + (lowByte & 0x000000ff);  // declared above as: int[] rawTemps = new int[254]
      }
      
      if (inBuffer[3] > 0) {                      //  if numDevices set to zero due to bad data, infinite loop ensues below.
        numDevices = int(inBuffer[3]);
      }
      else {
        numDevices = 1;
        println("number of devices not specified.");
      }
      
      dataMax = max(rawTemps); //needed for scaling data display
      startHour = int(inBuffer[0]);
      startMin = int(inBuffer[1]);
      startTimeSecs = startHour*60*60 + startMin*60;
      interval = int(inBuffer[2]);
      if (interval == 0) {
        intervalSecs = 2;
      } else {
        intervalSecs = interval * 60;
      }
      
      // calculate raw values and load into 2d array. first dimension = device, second dimension = raw tempvalue
      thermos = new int[numDevices][(ceil(rawTemps.length/numDevices))]; 
      println("new thermos array initiated. size is " + thermos.length + " by " + thermos[0].length);
   
      packTemps(rawTemps, thermos);
      
      printTemps(thermos); // for debugging purposes
   
           
      for (int i = 0; i < tempsIndex; ) {
        for (int j = 0; j < numDevices; j++) {
          print(i + ": " + rawTemps[i] + " (" + hex(rawTemps[i]) + ")" + "=> " + (rawTemps[i] * .0625) + " Deg C" + "\t");
          i++;
        }
        println();
      }
      

    } 
    else {
      println("Error> debug: " + byteCount + " bytes receieved. Bytes in buffer = " + myPort.available() );
    }
  dataReceived = true;  
  }

  myPort.clear();

} //end serialEvent
//**************************************************************************

void printData (byte[] data ) { // displays raw data received for debugging purposes
      println("started at hour:" + data[0] + ", minute: " + data[1] + ", interval: " + data[2] + " minute(s)");
      println("device count: " + data[3]);
      
      for (int i = 4; i < 512; i++) {
        print(i + ">" + hex(data[i]) + "; ");
      }
      println();
} // end printData  
//************************************************

void packTemps (int[] data, int[][] splitData) {
  int dataIndex = 0;
  
  //initialize splitData
  for (int i = 0; i < splitData[0].length; i++) {
    for (int j = 0; j < splitData.length; j++) {
      splitData[j][i] = MIN_INT;
    }
  }
  
  // pack splitData with data values
  for (int i = 0; i < splitData[0].length; i++){
    for (int j = 0; j < splitData.length; j++) {
      splitData[j][i] = data[dataIndex++];
    } 
  }

} // end packTemps
//**********************************************

void printTemps (int[][] data) {
  for (int col = 0; col < data.length; col++) {
    print(col + "\t\t");
  }
  
  println();
  
  for (int i= 0; i < data[0].length; i++) {
    for (int j= 0; j < data.length; j++) {
      print(data[j][i] + "=> " + (data[j][i] * .0625) + " C\t");
    }
    println();
  }  
} // end printTemps
//************************************************************

void drawTempButton() {
  strokeWeight(1);
  textSize(20);
  
  int radius = 45;
  float x = 70;
  float y = (plotY2 - plotY1)/2 + plotY1;
  
  ellipseMode(CENTER);
  textAlign(CENTER, CENTER);
  
  if (!dataReceived) {
    stroke(100);
    fill(224);
    ellipse(x, y, radius, radius);
    
    fill(0); // color of text - black
    text("C/R", x, y - 4); // -4 is a fudge factor
  } else { // data plotting mode
      stroke(0);
      if (dist(x, y, mouseX, mouseY) < radius) { // gray out button on rollover)
        fill(200);
        if ((mousePressed == true) && ((millis() - lastClick) > 100)) { // toggle tempScale on button press
          tempScale *= -1; // toggle tempScale - gotta be a better yet concise way of doing this
          lastClick = millis();
        }
      } else {
        fill(255);
      }
      ellipse(x, y, radius, radius);
    
      fill(#5866FF);
      if (tempScale == C) {
        text("C", x, y - 4);
      } else if (tempScale == F) {
        text("F", x, y - 4);
      }
  }
} // end drawTempButton
// **************************************************************

void drawConnectButton() { //button apparent prior to connection and receiving data. 
                           //will be replaced by data save, data load, and reset buttons.
  strokeWeight(1);  //thin lines
  textSize(15);
  textAlign(CENTER, CENTER);
  
  float x = buttonBaseX;
  float y = buttonBaseY;
  rectMode(CENTER);

  stroke(100); // somewhat dark gray border for box
  fill(224); // interior of box set to light default background
  
  
  
  if (!dataReceived) {
    if (((mouseX > x - buttonWidth/2) && (mouseX < x + buttonWidth/2)) && ((mouseY > y - buttonWidth/2) && (mouseY < y + buttonWidth/2)))  {
      fill(#5866FF);  // color of field changes to blue on rollover
      if (mousePressed && (millis() - lastClick > 200 )) {
        connect();
        lastClick = millis();
      }
    }
    rect(x, y, buttonWidth, buttonHeight);
    
    fill(0); //set color of text to black
    text("CONNECT", x, y-2);  

  } else {  // switch to Reset mode
    if (((mouseX > x - buttonWidth/2) && (mouseX < x + buttonWidth/2)) && ((mouseY > y - buttonWidth/2) && (mouseY < y + buttonWidth/2)))  {
      fill(#5866FF);  // color of field changes to blue on rollover
      if (mousePressed && (millis() - lastClick > 200 )) {
        dataReceived = false;
        lastClick = millis();
        thermos = null;
        println("thermos array cleared");
        myPort.clear();
      }
    }
    rect(x, y, buttonWidth, buttonHeight);
    
    fill(0); //set color of text to black
    text("RESET", x, y-2);  

    
  }
} // end drawConnectButton
// **************************************************************

void drawSaveDataButton() {
  
} // end drawSaveDataButton
// ***************************************************************

void drawDataPoints(int[] data) {

  stroke(#5679C1);
  
  for (int i = 0; i < data.length; i++) {   // data.length gives number of measurements
    strokeWeight(5);
    float y = map(data[i], dataMin, dataMax, plotY2, plotY1);
    float x = map(i, 0, data.length, plotX1, plotX2);
    
    // show highlight if mouse near data point
    if (dist(x, y, mouseX, mouseY) < 3) {
      strokeWeight(7);
      point(x, y);
      fill(0);
      textSize(10);
      textAlign(CENTER);
      if (tempScale == C) {
        text(str(data[i] * .0625), x, y - 8);
      } else if (tempScale == F) {
        text(str(cToF(data[i] * .0625)), x, y - 8);
      }
      textSize(20);
    } else {
      point(x, y);
    }
  }
  
}// end drawDataPoints
// ***********************************************************

void drawTimeLabels (int dataLength, int numColumns) {
  int scaler = dataLength/numColumns;
  int timeSecs;
  
  fill(0);
  textSize(10);
  textAlign(CENTER, TOP);
  
  stroke(224);
  strokeWeight(1);
  
  for (int i = 0; i < dataLength; i++) {
    if (i % scaler == 0) {
      float x = map(i, 0, dataLength, plotX1, plotX2);
      line(x, plotY1, x, plotY2);
      timeSecs = startTimeSecs + i*intervalSecs;
      text(getTimeStr(timeSecs), x, plotY2+7);
    }
  }
} // end drawTimeLabels
//***********************************************************

String getTimeStr (int seconds) {
// returns string representing time in 24 hour format. seconds is number of seconds elapsed since midnight.
  String timeStr, hoursStr, minsStr;
  int hours, mins;
  int totalMins;
 
  totalMins = seconds/60;
  hours = totalMins/60;
  mins = totalMins%60;
  
  if (hours < 10) {
    hoursStr = "0" + str(hours);
  } else {
    hoursStr = str(hours);
  } 
  
  if (mins < 10) {
    minsStr = "0" + str(mins);
  } else {
    minsStr = str(mins) ;
  }
  
  timeStr = hoursStr + minsStr;
  return timeStr;
} // end getTimeStr
//**************************************************************  

void drawTempLabelsC (){
// draw templabels along vertical axis. display temperature at intervals of vertInterval degrees (C)
  fill(0);
  textSize(10);
  textAlign(RIGHT, CENTER);
  
  stroke(224);
  strokeWeight(1);
  
  int vertInterval = 5;
  
  //start first label at a convenient integer value
  float firstVertVal = floor(dataMin * .0625 / vertInterval) * vertInterval  + vertInterval; // may not work so well if use floor other than 0 deg C
  float labelVal = firstVertVal;
 
  // draw bottom temp value
  text(str(dataMin * .0625), plotX1 - 10, plotY2);
  //line(plotX1 - 4, plotY2, plotX1, plotY2); //replaced with horizontal grid lines below (old stroke value was 128)
  
  //draw top value
  text(str(dataMax * .0625), plotX1 - 10, plotY1);
  //line(plotX1 - 4, plotY1, plotX1, plotY1); //replaced with horizontal grid lines
  
  // draw remaining values
  while(labelVal < dataMax *.0625) {
    float y = map(labelVal, dataMin * .0625, dataMax * .0625, plotY2, plotY1);
    text(str(labelVal), plotX1 - 10, y);
    line(plotX1, y, plotX2, y);
    labelVal += vertInterval;
  } 
} // end drawTempLabelsC
// **************************************************************************

void drawTempLabelsF (){
// draw templabels along vertical axis. display temperature at intervals of vertInterval degrees (C)
  fill(0);
  textSize(10);
  textAlign(RIGHT, CENTER);
  
  stroke(224);
  strokeWeight(1);
  
  int vertInterval = 5;
  
  //start first label at a convenient integer value
  float firstVertVal = floor(cToF(dataMin * .0625) / vertInterval) * vertInterval  + vertInterval; // may not work so well if use floor other than 0 deg C
  float labelVal = firstVertVal; 
 
  // draw bottom temp value
  text(str(cToF(dataMin * .0625)), plotX1 - 10, plotY2);
  //line(plotX1 - 4, plotY2, plotX1, plotY2); //produced short tick. replaced with horizontal grid lines below (old stroke value was 128)
  
  //draw top value
  text(str(cToF(dataMax * .0625)), plotX1 - 10, plotY1);
  //line(plotX1 - 4, plotY1, plotX1, plotY1); //replaced with horizontal grid lines
  
  // draw remaining values
  while(labelVal < cToF(dataMax *.0625)) {
    float y = map(labelVal, cToF(dataMin * .0625), cToF(dataMax * .0625), plotY2, plotY1);
    text(str(labelVal), plotX1 - 10, y);
    line(plotX1, y, plotX2, y);
    labelVal += vertInterval;
  } 
} // end drawTempLabelsF
// **************************************************************************  

float cToF(float cValue) {
  return (cValue * 1.8) + 32;
} // end cToF 
// ***************************************************************************


