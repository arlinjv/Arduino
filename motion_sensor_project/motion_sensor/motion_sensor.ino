/*
 * Basic concept of operation:
 *  - On activation sends GET request to Flask server
 *    - GET request format - TBD
*     
*  Future features:
*    - Also be available for queries from server
*       - status check
*       - readings on other sensors
*   - periodic "I'm here" messages or respond to watchdog type messages
*     
*  For testing GET request sending run SimpleHTTPServer: 'python -m SimpleHTTPServer 5000'
*  
*  To do:
*   - add a handler for setting host and port
*     
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h> // consider replacing with ESP8266HTTPClient.h - approach seems to be more consistent with ESP8266WebServer methods - see library examples
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h> // no idea if or why I need this
#include <ESP8266WiFiMulti.h>

// WiFi related declarations: --------------------------------------------------
const char* host = "192.168.1.67"; //on ATT226
//const char* host = "192.168.50.124"; // on i-GATE
const int httpPort = 5000; // standard Flask port

const char* deviceID = "esp2";
const char* sensorID = "ms1";

ESP8266WiFiMulti wifiMulti;
ESP8266WebServer server(80);
WiFiClient client; // should still need for connecting to another website

// Hardware declarations: ------------------------------------------
const int sensorPin = 14;

// prototypes
bool    sensorActivated();
void    sendReading();
void    handleRoot();
void    handleNotFound();
void    handleStatus();
void    handleReset();
void    handleLastActivationRequest();

//globals
unsigned long lastActivation = 0;


void setup() {
  pinMode(sensorPin, INPUT);

  Serial.begin(115200);
  //Serial.setDebugOutput(true); //using this may conflict with libraries that use printf - like in the WiFiClientEvents.ino example
  Serial.println(); // to keep output from getting stuck at end of opening gibberish

  delay(500);

  wifiMulti.addAP("ATT226", "3902845518");
  wifiMulti.addAP("i-GATE", "2324second");

  Serial.println("Connecting Wifi...");

    while(wifiMulti.run() != WL_CONNECTED){
    Serial.println("Wifi not connected");
    delay(1000);
  }
  
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  long rssi = WiFi.RSSI();
  Serial.print("RSSI: "); Serial.println(rssi);
  Serial.print("Status: "); Serial.println(WiFi.status());
  
  Serial.println("--------------------------------");
  
  Serial.println("printDiag output: ");
  WiFi.printDiag(Serial);

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.on("/status", handleStatus);
  server.on("/last", handleLastActivationRequest);
  server.on("/reset", handleReset);
  server.begin();
  Serial.println("HTTP server started");  


}  // ----------------------- end setup() ---------------------------------

void loop() {
  delay(1);

  server.handleClient();
  
  if (sensorActivated()){
    Serial.println();
    Serial.println("--------------Sending reading--------------");
    sendReading();
  }

} // ---------- end loop() -----------------

bool sensorActivated(){
  static bool sensorActive = false;
  static bool sensorOldState = false;
  
  if (digitalRead(sensorPin) == HIGH){
    sensorActive = true;
  }
  else {
    sensorActive = false;
  }

  if (sensorActive == false and sensorOldState == true){
    sensorOldState = false;
    return false;
  }

  if (sensorActive == true and sensorOldState == false){
    sensorOldState = true;
    lastActivation = millis();
    return true;
  }

  return false;

}

void sendReading(){
  // For sending data to remote server like enviro_monitor (logger.py)
  // Most appropriate when sending periodic or unsolicited data (i.e. working as an alarm) 

  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return; // switch to break or add connection fail parameter to stop spamming server
  }

  
  // We now create a URI for the request - see here for more on formatting get requests: https://www.jmarshall.com/easy/http/http_footnotes.html#getsubmit
  String url = "/"; //this will change based on receiving api. e.g. "/alarms" or  "/testwifi/index.html";

  /*url += "?value=";
  url += value;
  url += "&millis=";
  url += millis(); */

  // basic sensor message format device ID, sensor ID, sensor type, sensor value, units
  // 
  url += "alarm?device_ID="; url += deviceID;
  url += "&sensor_ID="; url += sensorID;
  url += "&sensor_type=motion";
  url += "&sensor_value=true"; // could just as easily be '1' or 'none' or 'n/a'
  url += "&units=bool";
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  String requestString = String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n";             
  Serial.println("Request string: ");
  Serial.println(requestString);        
  client.print(requestString);
  
  
  unsigned long bt = millis();
  while(client.connected() && !client.available()) {
    delay(1); //waits for data
    if (millis() - bt > 5000){
      Serial.println("Timed out waiting for response"); // !!!!!! add code to reset connection if timed out
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  Serial.println("Server response:");
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("------ closing connection -------");
  Serial.println();
  client.stop();
  
} // end sendReading(); -------------------------------------------------
