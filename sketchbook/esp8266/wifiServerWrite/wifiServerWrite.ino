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

  WiFiClient client = server.available();
  if (client == true) {
       // read bytes from the incoming client and write them back
       // to any clients connected to the server:
    Serial.println("Incoming client connected.");

    String request = client.readStringUntil('\r');
    Serial.print("Request: "); Serial.println(request);
    client.print("Backatchya: "); client.println(request);

  }

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
