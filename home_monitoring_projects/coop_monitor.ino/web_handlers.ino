String getTimeString(unsigned long milliseconds){
  
  int sec = milliseconds / 1000;
  int min = sec / 60;
  int hr = min / 60;

  String text = String(hr) + ":" + String(min%60) + ":" + String(sec%60); // this should work - not worth testing rigorously

  return text;
}
 
void handleRoot() { // reworked from esp_temp_monitor

  String page = "";

  Serial.print("status command: ");
  Serial.println(server.arg("status"));

  if (server.arg("status") == "ON"){
    status = ON;
    digitalWrite(controlPin, HIGH);
    Serial.println("received ON command via webserver");
  } 
  else if (server.arg("status") == "OFF"){
    status = OFF;
    digitalWrite(controlPin, LOW);
    Serial.println("received OFF command via webserver");
  } 

  statusString = (status == ON) ? "ON" : "OFF";

  String mqttConnectionStatus = (client.connected()) ? "connected" : "connected";

// add mqtt server address and status

  page +="<html>";
  page += "<head>";
  page +=   "<title>Master Irrigation Controller</title>";
  page +=   "<style>";
  page +=     "body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }";
  page +=     "table, th, td {border: 1px solid black;border-collapse: collapse;}";
  page +=     "th {text-align: left;}";
  page +=     "th, td {padding: 15px;}";
  page +=   "</style>";
  page += "<body>";
  page +=   "<h1>Master Irrigation</h1>";
  page +=   "<h2>Status: " + statusString + "</h2>";
  page +=   "<p></p>";
  page +=   "<form action=\"/\" method=\"get\">";
  page +=   "Set status:";
  page +=   "<button name=\"status\" type=\"submit\" value=\"ON\">ON</button>";
  page +=   "<button name=\"status\" type=\"submit\" value=\"OFF\">OFF</button>";
  page +=   "</form>";
//mqttServer // 1883
//page +="<p>mqtt: " + mqttConnectionStatus + "at " + (String mqttServer) + 
  
  page +=   "<p>Uptime: " + getTimeString(millis()) + "</p>";
  page +=   "<p></p>";
  page += "</body>";
  page +="</html>";
  
  server.send ( 200, "text/html", page );
} // end handleRoot() +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void handleStatus(){ // returns uptime
  // do like in handleroot() but just display relay status and refresh every n seconds
  String message = String(millis()); // <-- replace with something meaningful, maybe something about device memory?
  server.send(200, "text/plain", message);
  Serial.println("status request received");
}

void handleMasterOn(){
  String message = "turning ON master irrigation control";
  server.send(200, "text/plain",message);
  Serial.println("master irrigation ON command received via webserver.");
  digitalWrite(controlPin, HIGH);
  status = ON;
}

void handleMasterOff(){
  String message = "turning OFF master irrigation control";
  server.send(200, "text/plain",message);
  Serial.println("master irrigation OFF command received via webserver.");
  digitalWrite(controlPin, LOW);  
  status = OFF;
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



