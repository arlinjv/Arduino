 /*
 *  Simple HTTP get webclient test
 *  
 *  To test easily run use SimpleHTTPServer - python -m SimpleHTTPServer 5000
 *  
 *  To do:
 *    - Add server code
 *      - send rssi data upon prompt
 *      - 
 *    - add connection fail parameter to stop spamming server by only looping so many times before giving up
 *      - have a fail count loop and blink light when no connection (have different patterns for AP and server connection problems)
 *      - maybe prompt user for new ip and host
 *    - Add code to try different AP if first didn't work (use timeout code from here: https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/WiFiClient/WiFiClient.ino)
 *      - or maybe just switch to AP mode and expose configuration menu. would have to use progmem or similar
 *        - isn't there a library for this?
 *    - Add checkin code - change settings (like transmit frequency) based on return from server
 *      - maybe have code for a special check in switch so that node will send a check in message to server
 *    
 *  Done:
 *    - Add timer to see how fast wifi connects from startup (about 2.3 seconds)
 *    - Add prompt to pause before continuing loop
 *    - print request string to console
 *    - Add get request 
 *    
 */

#include <ESP8266WiFi.h>

//const char* ssid     = "i-GATE";
//const char* password = "2324second";
//const char* host = "192.168.50.124";
 
const char* ssid      = "ATT226";
const char* password  = "3902845518";
const char* host = "192.168.1.67";

const int httpPort = 5000;

const char* deviceID = "esp1";

const int analogInPin = A0; 

WiFiServer server(80);
WiFiClient client; // not sure if needed here

void connectToAP(); // prototype (appears IDE isn't autoprototyping for ESP8266. may already be fixed in new builds - https://github.com/esp8266/Arduino/issues/1066)

void setup() {

  Serial.begin(115200);
  delay(100);

  // We start by connecting to a WiFi network
  connectToAP();
  
  server.begin(); // connectToAP() should block until connection is made
  
}

void loop() {
  //delay(5000); // take this out if no problems - think it was here because original sketch kept looping without prompt


  Serial.println("Checking for incoming client");
  WiFiClient client = server.available();
  if (client == true) {
       // read bytes from the incoming client and write them back
       // to any clients connected to the server:
    Serial.println("Incoming client connected.");

    client.read()
    server.write(client.read());
  }
  
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
  
  delay(500);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection - type something to send next message");
  Serial.println();
  
  while(!Serial.available());
  char ch = Serial.read(); // throw away - multiple key preses give multiple loops

}


void connectToAP() {
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
}
