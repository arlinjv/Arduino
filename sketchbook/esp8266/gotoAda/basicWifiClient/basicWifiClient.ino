 /*
 *  Simple HTTP get webclient test
 *  To do:
 *    - add connection fail parameter to stop spamming server by only looping so many times before giving up
 *      - have a fail count loop and blink light when no connection (have different patterns for AP and server connection problems)
 *    - Add code to try different AP if first didn't work (use timeout code from here: https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/WiFiClient/WiFiClient.ino)
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
 
const char* ssid      = "ATT226";
const char* password  = "3902845518";

const char* host = "192.168.1.67";

const int httpPort = 5000;

const char* deviceID = "esp1";

const int analogInPin = A0;  // according to 
int startupTime;

void setup() {
  startupTime = millis();
  
  Serial.begin(115200);
  delay(100);

  // We start by connecting to a WiFi network

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
  Serial.print("IP address (local): ");
  Serial.println(WiFi.localIP());

  int timeToConnect = millis() - startupTime;
  Serial.print("Time to connect to Wifi AP from startup: ");
  Serial.println(timeToConnect);
  Serial.println();
  
}

int value = 0;

void loop() {
  //delay(5000); // take this out if no problems - think it was here because original sketch kept looping without prompt
  
  ++value;
  
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return; // switch to break or add connection fail parameter to stop spamming server
  }

  int timeToConnectHost = millis() - startupTime;
  Serial.print("Time to connect to host: ");
  Serial.println(timeToConnectHost);

  int adc = analogRead(analogInPin);
  Serial.print("adc: ");
  Serial.println(adc);
  
  // We now create a URI for the request - see here for more on formatting get requests: https://www.jmarshall.com/easy/http/http_footnotes.html#getsubmit
  String url = "/"; // could be much longer. e.g. "/testwifi/index.html";

  /*url += "?value=";
  url += value;
  url += "&millis=";
  url += millis(); */

  // basic sensor message format device ID, sensor type, sensor value, units
  url += "?ID=";
  url += deviceID;
  url += "&type=light";
  url += "&value=";
  url += adc;
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

  startupTime = millis();

}

