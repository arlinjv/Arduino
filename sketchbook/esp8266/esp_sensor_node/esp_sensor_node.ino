 /*
 *  Simple HTTP get webclient test
 *  
 *  To test easily run use SimpleHTTPServer - python -m SimpleHTTPServer 5000
 *  
 *  To do:
 *    - Add wifimulti code for connecting to various APs - https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/WiFiMulti/WiFiMulti.ino
 *    - Add LED feedback for various situations.
 *      - red - pin 0, blue - pin 2  e.g.: pinMode(0, OUTPUT); (in setup) then digitalWrite(0, HIGH);
 *    - Add server code
 *      - send rssi data upon prompt
 *      - 
 *    - add connection fail parameter to stop spamming server by only looping so many times before giving up
 *      - have a fail count loop and blink light when no connection (have different patterns for AP and server connection problems)
 *      - maybe prompt user for new ip and host
 *    - Add code to try different AP if first didn't work (use timeout code from here: https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/WiFiClient/WiFiClient.ino)
 *      - or maybe just switch to AP mode and expose configuration menu. would have to use progmem or EEPROM
 *        - isn't there a library for this?
 *    - Add checkin code - change settings (like transmit frequency) based on return from server
 *      - maybe have code for a special check in switch so that node will send a check in message to server
 *    
 *  Done:
 *    - Add timer to see how fast wifi connects from startup (about 2.3 seconds)
 *    - Add prompt to pause before continuing loop (dropped)
 *    - print request string to console
 *    - Add get request 
 *    
 */

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

//const char* ssid     = "i-GATE";
//const char* password = "2324second";
//const char* host = "192.168.50.124";
 
const char* ssid      = "ATT226";
const char* password  = "3902845518";
const char* host = "192.168.1.67";

const int httpPort = 5000;

const char* deviceID = "esp1";

const int analogInPin = A0; 

ESP8266WiFiMulti wifiMulti;

WiFiServer server(80);
String request;

WiFiClient client; // not sure if needed here

void    connectToAP(); // prototype (appears IDE isn't autoprototyping for ESP8266. may already be fixed in new builds - https://github.com/esp8266/Arduino/issues/1066)
void    sendReading();
String  getRequest(WiFiClient);


void setup() {

  Serial.begin(115200);
  delay(100);

  // We start by connecting to a WiFi network
  connectToAP();
  
  server.begin(); // connectToAP() should block until connection is made
  
}

void loop() {

  WiFiClient client = server.available();
  if (client == true) {

    request = getRequest(client);
    int pathIndex = request.indexOf('/');
    String path = request.substring(pathIndex + 1); // strip off leading slash, too
    Serial.print("request path: "); Serial.println(path);

    // Menu here. Some ideas: rssi, diag (wifi.printDiag(Serial), uptime, poll, change parameters (polling rate,...)
    
    sendReading();

  }

} // ---------- end loop() -----------------

String getRequest(WiFiClient client){ // Seems I need to pass client object. I guess this is good form but not sure why I can't just refer to the global object
  // read bytes from the incoming client and write them back
  // to any clients connected to the server:
  Serial.println("Incoming client connected.");

  String rq = client.readStringUntil('\r');
  Serial.print("Request: "); Serial.println(rq);
  Serial.println(" ---------------------- ");
  client.print("Request received - "); client.println(rq);

  delay(1);
  //stopping client
  client.stop();

  return(rq);


}

void connectToAP() {
  // To do: implement wifiMulti (library is included but not using here - see esp_temp_monitor)
  // To do: add timeout code. maybe take timeout time as parameter. will need to change to boolean type and return connection status
  int startTime = millis();
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.print("IP address (local): "); Serial.println(WiFi.localIP());
  long rssi = WiFi.RSSI();
  Serial.print("RSSI: "); Serial.println(rssi);
  Serial.print("Time to connect to Wifi AP: "); Serial.println(millis() - startTime);
  Serial.println();
  Serial.println("==========================================");
}

void sendReading(){
  // For sending data to remote server like enviro_monitor (logger.py)
  // Should mostly be useful when working on alarm or periodic basis
  // When polled directly by remote server may make more sense to return data directly
  
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return; // switch to break or add connection fail parameter to stop spamming server
  }

  int adc = analogRead(analogInPin);
  Serial.print("adc: ");
  Serial.println(adc);
  
  // We now create a URI for the request - see here for more on formatting get requests: https://www.jmarshall.com/easy/http/http_footnotes.html#getsubmit
  String url = "/"; // could be much longer. e.g. "/testwifi/index.html";

  /*url += "?value=";
  url += value;
  url += "&millis=";
  url += millis(); */

  // basic sensor message format device ID, sensor ID, sensor type, sensor value, units
  url += "add?device_ID="; url += deviceID;
  url += "&sensor_ID=ls1";
  url += "&sensor_type=light";
  url += "&sensor_value="; url += adc;
  url += "&units=raw";
  
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
      Serial.println("Timed out waiting for response");
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");
  Serial.println();
  client.stop();
  
}

