  String getTempTableRows(){ 
    //Used below to populate table rows. Would put in handleRoot() but apparently nested functions aren't allowed.
    String rowString = "";

    for(int i = 0; i < numberOfDevices; i++){
      rowString +="<tr>"; 
      rowString +="<td>" + String(i) + "</td>";
      rowString +="<td>" + String(thermos[i].tempC) + "</td>";
      rowString +="<td>" + thermos[i].address + "</td>";
      rowString +="</tr>";
    }
    return rowString;
  }

void handleRoot() {
  // to do:
  //    - add refresh button
  //        - by linking back here
  //        - or by javascript 

  updateTemps();
  
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  String page = "";

  static int refreshRate = 5;
  int newRate = String(server.arg("refreshRate")).toInt();
  if (newRate > 0){
    refreshRate = newRate;
  }

  page +="<html>";
  page += "<head>";
  page +=   "<meta http-equiv='refresh' content='" + String(refreshRate) + "'/>"; // refresh browser page every five seconds
  page +=   "<title>Temperature Monitor</title>";
  page +=   "<style>";
  page +=     "body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }";
  page +=     "table, th, td {border: 1px solid black;border-collapse: collapse;}";
  page +=     "th {text-align: left;}";
  page +=     "th, td {padding: 15px;}";
  page +=   "</style>";
  page += "<body>";
  page +=   "<h1>Current Temperatures</h1>";
  page +=   "<table style=\"width:50%\">";
  page +=     "<tr>";
  page +=       "<th>Device #</th>";
  page +=       "<th>Temperature (C)</th>";
  page +=       "<th>Address</th>";
  page +=     "</tr>";
  page +=   getTempTableRows();
  page +=   "</table>";
  page +=   "<p></p>";
  page +=   "<form action=\"/\" method = \"get\"> Refresh rate:<input type=\"text\" name=\"refreshRate\">";
//  page +=     "<br><input type=\"submit\" value=\"Submit\">";
  page +=   "</form>";
  page +=   "<form method=\"get\" action=\"/\"><button type=\"submit\">Reload</button></form>";
  page +=   "<img src=\"/test.svg\" />";
  page +=   "<p>Uptime: " + String(hr) + ":" + String(min) + ":" + String(sec) + "</p>"; // this must be wrong - there should be some mods in here
  page +=   "<p> Refresh rate: " + String(refreshRate) + "</p>";
  page += "</body>";
  page +="</html>";
  
  server.send ( 200, "text/html", page );
} // end handleRoot() +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args(); // returns number of arguments
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.print("Unable to handle this request");
  Serial.println(message);
}

void handleDataRequest(){
  server.send(200, "text/plain", getLDRString());
  Serial.println("data request received");
}

void drawGraph() {
  String out = "";
  char temp[100];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"400\" height=\"150\">\n";
  out += "<rect width=\"400\" height=\"150\" fill=\"rgb(250, 230, 210)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<g stroke=\"black\">\n";
  int y = rand() % 130;
  for (int x = 10; x < 390; x+= 10) {
    int y2 = rand() % 130;
    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", x, 140 - y, x + 10, 140 - y2);
    out += temp;
    y = y2;
  }
  out += "</g>\n</svg>\n";

  //Serial.print("svg string: "); Serial.println(out);

  server.send ( 200, "image/svg+xml", out);
}

/*
 * +++++++++++++++++++++++++++++++++++++ Snippets +++++++++++++++++++++++++++++++++++++++

// handleRoot() from AdvancedWebserverExample

void handleRoot() { 
  // abandoning this char array version:
  // - dynamically constructing page may not be practical
  // - difficulty with representing floats see here: http://dereenigne.org/arduino/arduino-float-to-string
  
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  updateTemps();

  Serial.print("Thermo 1: ");
  Serial.println(thermos[0].tempC);
  
  snprintf ( temp, 400,

"<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>Temperature monitor</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Current Temperatures</h1>\
    <h1>Thermo 1 %03d</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
  </body>\
</html>",

    thermos[0].tempC, hr, min % 60, sec % 60
  );
  server.send ( 200, "text/html", temp );
}

// - old handleRoot()
void handleRoot() { // for some reason IDE doesn't recognize prototype for this so have to leave here.
  String responseString = ("hello from esp8266! ");
  responseString += millis();
  server.send(200, "text/plain", responseString);
}
 */
