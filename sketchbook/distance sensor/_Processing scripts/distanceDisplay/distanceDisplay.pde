import processing.serial.*;

Serial myPort;

PFont plotFont;

void setup() {
  size(820, 505);

  plotFont = createFont("SansSerif", 20);
  textFont(plotFont);
  
  smooth();

  myPort = new Serial(this, "COM8", 9600);
}

void draw() {
 background(224);

}
