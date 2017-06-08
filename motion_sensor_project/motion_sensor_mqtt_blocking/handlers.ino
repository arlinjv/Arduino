String getTimeString(unsigned long milliseconds){
  
  int sec = milliseconds / 1000;
  int min = sec / 60;
  int hr = min / 60;

  String text = String(hr) + ":" + String(min%60) + ":" + String(sec%60); // this should work - not worth testing rigorously

  return text;
}

void handleRoot() {
  // make an html page here:
  // show status of sensors
  // add reset button

  int sec = (millis()) / 1000;
  int min = sec / 60;
  int hr = min / 60;
  
  String message = "";
  message += "uptime                     - " + getTimeString(millis()) + "\n";
  message += "time since last activation - " + getTimeString(millis()-lastActivation);
  
  server.send(200, "text/plain", message);
  Serial.println("device root accessed");

}

void handleStatus(){ // returns uptime
  String message = String(millis());
  server.send(200, "text/plain", message);
  Serial.println("status request received");
}

void handleLastActivationRequest(){
  String message = String(millis()-lastActivation);
  server.send(200, "text/plain", message);
  Serial.println("status request received");  
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



