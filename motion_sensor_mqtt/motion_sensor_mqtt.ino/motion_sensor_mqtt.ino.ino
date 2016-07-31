/* Basic concept:
 *  - connect to mqtt server
 *  - publish sensor activations to relevant topic
 *  - run webserver for status and configuration purposes.
 *  
 *  pubs:
 *    - portables/esp2/motion  (seems to be a special case as in there is really no need for a payload)
 *    - portables/esp2/status > send upon connected and when request received through status subscription
 *  
 *  
 *  subs
 *    - portables/esp2/led/PUT on/off (or maybe just portables/esp2/led/+ and parse for verb and go from there)
 *    - portables/esp2/GET  (here unit itself is the resource. Respond to payload, e.g. 'status,' This as opposed to portables/esp2/status/GET where payload would be irrelevant)
 *    -
 *    
 * Notes:
 *  - pubsub library:
 *    - sketch will reconnect to the server if the connection is lost using a blocking
 *     reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to achieve the same result without blocking the main loop.
 *  - 
 *  
 * To do:
 *  - (done) switch reconnect to nonblocking method (see pub_sub example)
 *  - use EEPROM (or spi_flash_write and spi_flash_read) to save configuration values
 *  - create webserver interface for updating configuration settings.
 *  - implement a subscription for some sort of broadcast channel (for system wide pings perhaps)
 *  - implement a handler for listing resources (motion, light, temp, etc.)
 *  - have list of subs and pubs. 
 *    - on reconnect loop through subs to subscribe to each in turn.
 * 
 */

 
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h> // no idea if or why I need this
#include <ESP8266WiFiMulti.h>
#include <PubSubClient.h>

const char* deviceID = "esp2";
const char* sensorID = "ms1"; // may want to drop this

ESP8266WiFiMulti wifiMulti;
ESP8266WebServer server(80);

const char* mqtt_server = "192.168.1.67";
WiFiClient espClient;
PubSubClient client(espClient);
char msg[50]; // for publish

// Hardware declarations: ------------------------------------------
const int sensorPin = D7;

// prototypes
void setup_wifi();
void callback(char* topic, byte* payload, unsigned int length);
bool sensorActivated();
void sendReading();
void handleRoot();
void handleNotFound();
void handleStatus();
void handleReset();
void handleLastActivationRequest();
boolean reconnect();

//globals
unsigned long lastActivation = 0;
long lastReconnectAttempt = 0;


void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
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

  client.setServer(mqtt_server, 1883);  // mqtt server (not http)
  client.setCallback(callback);         // handles incoming messages


}  // ----------------------- end setup() ---------------------------------

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (strcmp(topic, "portables/esp2/led/PUT") == 0) {
    Serial.println("led command received");
  }

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}  // ------------------------- end callback() ----------------------------

boolean reconnect() {
  
   if (client.connect("ESP8266Client")) {
    // Once connected, publish an announcement...
    client.publish("portables/esp2/status","Esp2 connected");
    // ... and resubscribe
    client.subscribe("portables/esp2/led/PUT");
    client.subscribe("portables/esp2/GET");
  }
  return client.connected();
  
}  // ----------------------------- end reconnect() ------------------------

// *****************************************************************************
void loop() {
  delay(1);

  // mqtt:
  if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 1000) {
      lastReconnectAttempt = now;
      Serial.println("attempting to connect mqtt");
      if (reconnect()) {// Attempt to reconnect
        Serial.println("mqtt connected");
        lastReconnectAttempt = 0;
      }
    }
  } else {// Client connected
    client.loop();
  } 

  // webserver - for checking status and (to do) configuration
  server.handleClient();
  
  if (sensorActivated()){
    Serial.println();
    Serial.println("publishing to portables/esp2/motion: sensor activated");
    client.publish("portables/esp2/motion", "sensor activated");
  }

} // ---------- end loop() -----------------
// ***************************************************************************

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
  
} // end sendReading(); -------------------------------------------------
