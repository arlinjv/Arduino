String getTimeString(unsigned long milliseconds){
  
  int sec = milliseconds / 1000;
  int min = sec / 60;
  int hr = min / 60;

  String text = String(hr) + ":" + String(min%60) + ":" + String(sec%60); // this should work - not worth testing rigorously

  return text;
}
 
void handleRoot() { // reworked from esp_temp_monitor

  String page = "";

  static int refreshRate = 5;
  int newRate = String(server.arg("refreshRate")).toInt();
  if (newRate > 0){
    refreshRate = newRate;
  }

  page +="<html>";
  page += "<head>";
  page +=   "<meta http-equiv='refresh' content='" + String(refreshRate) + "'/>"; // refresh browser page every five seconds
  page +=   "<title>Tank Monitor</title>";
  page +=   "<style>";
  page +=     "body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }";
  page +=     "table, th, td {border: 1px solid black;border-collapse: collapse;}";
  page +=     "th {text-align: left;}";
  page +=     "th, td {padding: 15px;}";
  page +=   "</style>";
  page += "<body>";
  page +=   "<h1>Water Level: " + msgString + "</h1>";
  page +=   "<p></p>";
  page +=   "<form action=\"/\" method = \"get\"> Refresh rate:<input type=\"text\" name=\"refreshRate\">";
  page +=   "</form>";
  page +=   "<form method=\"get\" action=\"/\"><button type=\"submit\">Reload</button></form>";
  page +=   "<p>Uptime: " + getTimeString(millis()) + "</p>";
  page +=   "<p> Refresh rate: " + String(refreshRate) + "</p>";
  page += "</body>";
  page +="</html>";
  
  server.send ( 200, "text/html", page );
} // end handleRoot() +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void handleStatus(){ // returns uptime
  String message = String(millis()); // <-- replace with something meaningful, maybe something about device memory?
  server.send(200, "text/plain", message);
  Serial.println("status request received");
}

void handleLastReadingRequest(){
  server.send(200, "text/plain", msg);
  Serial.println("last reading requested");  
}

void handleReset(){
  // notes on Reset:
  // known bug involving getting stuck at wdt restart after first programming - need to unpower and restart
  // might be best to tie GPIO16 to reset pin. Also apparently some issues with GPIO2.
  String message = "Resetting now";
  server.send(200, "text/plain", message);
  Serial.println("Received reset request");
  Serial.println("Resetting now");
  ESP.reset();
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}



