/* Equipment:
 *    - Amica labelled esp8266 (use nodemcu board setting)
 *    - wired power (for now): 9v? 24v AC?
 *  
 *  Basic concept:
 *  - monitor chicken coop:
 *    - door
 *    - temperature
 *    - sound levels
 *    - egg presence
 *  - Control environment:
 *    - misters
 *    - open door     
 *  - connect to mqtt server
 *  - respond to mqtt commands
 *  - run webserver for status control and configuration purposes.
 *  
 *  pubs: 
 *    (alternate root will be 'house'. No need for 'home' unless monitoring other locations)
 *    - garden/coop > broadcast status events here. Differentiate type of event in payload  (e.g. {"event_type":"connected"} or {"event_type":"sensor activation", "resource":"main_door", "value":"open")
 *    - garden/census > send identifying information when pinged at garden/census/ping > payload:{"address":"garden/coop","url":"192.....","resources":[]}
 *  
 *  subs
 *    - garden/coop/GET > reply on garden/coop. Respond to payload, e.g. 'status' or 'main_door' (Alternate scheme would be to listen on garden/coop/status/GET)
 *    - garden/coop/mister/PUT - 'on' or 'off' to control master solenoid valve (maybe more consistent to do garden/coop/PUT: just like GET but differentiate active and passive resources)
 *    - garden/census/ping - all listening devices answer with an "I'm here"-type response
 *    
 * Notes:
 *  - Based on master_irrigation_MQTT.ino which was intended for robot garden fish tank
 *  - To test mqtt:
 *    - check if mosquitto is running (run mosquitto in a terminal. you'll get an error if it' already running)
 *      or better: run >pgrep mosquitto
 *    - subscribe: mosquitto_sub -t garden/tank-monitor/water-level -d // -d is for debugging - leave off for cleaner output - alternately use -v for verbose
 *      or: mosquitto_sub -h 192.168.1.67 -t "#" -v (listen to everything in verbose mode
 *    - publish: mosquitto_pub -t 'garden/master-irrigation/led/PUT' -m '0'
 *  - pubsub library (http://pubsubclient.knolleary.net/api.html):
 *    - sketch will reconnect to the server if the connection is lost using a non-blocking reconnect function. (See the 'mqtt_reconnect_nonblocking' example)  
 *  
 * To do: (includes leftovers from master irrigation sketch)
 *  - drop enum code in globals?
 *  - add failsafe - turn off relay when lose connection
 *  - update client.publish(...) in reconnect() - should provide full status string 
 *  - remove msgString code
 *  - mqtt:
 *    - implement GET (and PUT) callbacks for 
 *      - ping 
 *      - status (server ip address, uptime, controller state
 *    - pretty up mqttCallback by wrapping strcmp lines in simpler handler
 *  - use EEPROM (or spi_flash_write and spi_flash_read) to save configuration values
 *  - Webserver:
 *    - update status page (add option for adjusting reload frequency
 *    - create interface for updating configuration settings.
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
#include <ArduinoJson.h>

const char* deviceID = "coop_monitor";

ESP8266WiFiMulti wifiMulti;
ESP8266WebServer server(80);

//mqtt
const char* mqtt_server = "192.168.1.87"; // home - raspi1 connected by ethernet to router (as of 6/8/17)
//const char* mqtt_server = "192.168.50.27"; // i-gate
WiFiClient espClient;
PubSubClient client(espClient);
char msg[50]; // for publish // use String object and reserve in setup instead?
//String msgString; // analogue to msg[50] - reserve 50 bytes in setup

//globals
long lastReconnectAttempt = 0;
const int controlPin = D8; // connects to relay
enum states { // ON = 1 OFF = 0
  ON,
  OFF
};
states status = OFF;
String statusString = "off";

// prototypes
//void setup_wifi(); - needed?
void mqttLoop();
void mqttCallback(char* topic, byte* payload, unsigned int length);
boolean reconnect();
void handleRoot();
void handleNotFound();
void handleStatus();
void handleReset();
void handleMasterOn();
void handleMasterOff();

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(controlPin, OUTPUT);
  digitalWrite(controlPin, LOW);

  //msgString.reserve(50);

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
  server.on("/reset", handleReset);
  server.on("/on", handleMasterOn);
  server.on("/off", handleMasterOff);
  server.begin();
  Serial.println("HTTP server started");  

  client.setServer(mqtt_server, 1883);      // mqtt server (not http)
  client.setCallback(mqttCallback);         // handles incoming messages


}  // ----------------------- end setup() ---------------------------------



// ********************************** LOOP *******************************************
void loop() {
  delay(1);

  // mqtt: reconnect if necessary otherwise client.loop()
  mqttLoop();

  // webserver - for checking status and (to do) configuration
  server.handleClient();

} // ---------- end loop() -----------------
// ***************************************************************************

