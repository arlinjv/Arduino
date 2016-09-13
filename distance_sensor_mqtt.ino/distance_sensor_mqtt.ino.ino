/* Basic concept:
 *  - take periodic distance measurements
 *  - connect to mqtt server
 *  - publish sensor activations to relevant topic
 *  - run webserver for status and configuration purposes.
 *  
 *  pubs: 
 *    (add rg/ to all addresses?)
 *    - garden/tank-monitor/water-level  
 *    - garden/tank-monitor/status > send upon connected and when request received through status subscription
 *    - garden/census/responses > send identifying information when pinged at garden/census/ping > payload:{"address":"garden/tank-monitor","url":"192.....","resources":[]}
 *  
 *  
 *  subs
 *    - garden/tank-monitor/GET  (here unit itself is the resource. Respond to payload, e.g. 'status' or 'distance'. This as opposed to portables/esp2/status/GET where payload would be irrelevant)
 *    - garden/tank-monitor/PUT - update some value (like measurement frequency)
 *    - garden/census/ping - all listening devices answer with an "I'm here"-type response
 *    
 * Notes:
 *  - To test:
 *    - check if mosquitto is running (run mosquitto in a terminal. you'll get an error if it' already running)
 *    - subscribe: mosquitto_sub -t garden/tank-monitor/water-level -d // -d is for debugging - leave off for cleaner output
 *    - publish: mosquitto_pub -t 'garden/tank-monitor/led/PUT' -m '0'
 *  - pubsub library (http://pubsubclient.knolleary.net/api.html):
 *    - sketch will reconnect to the server if the connection is lost using a non-blocking reconnect function. (See the 'mqtt_reconnect_nonblocking' example)  
 *    
 * Some issues to address:
 *  - readings may need to be filtered
 *    - readings outside of possible bounds need to be discarded outright.
 *    - other apparently valid readings should be averaged or median filtered.(?)
 *  
 * To do:
 *  - (done) switch to newPing library - discards out of range values, doesn't hang (doesn't use pulseIn), has options for non-blocking mode and median filtering
 *  - use EEPROM (or spi_flash_write and spi_flash_read) to save configuration values
 *  - Webserver:
 *    - (done) create a status page (to show current measurements) and add option for adjusting reload frequency
 *    - create interface for updating configuration settings.
 *  - implement a subscription for some sort of broadcast channel (for system wide pings perhaps)
 *  - implement a handler for listing resources (motion, light, temp, etc.)
 *  - have list of subs and pubs. 
 *    - on reconnect loop through subs to subscribe to each in turn.
 * 
 */

#include <NewPing.h> 
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h> // no idea if or why I need this
#include <ESP8266WiFiMulti.h>
#include <PubSubClient.h>

const char* deviceID = "tank-monitor";
const char* sensorID = "distance-sensor"; // may want to drop this

ESP8266WiFiMulti wifiMulti;
ESP8266WebServer server(80);

const char* mqtt_server = "192.168.1.67";
WiFiClient espClient;
PubSubClient client(espClient);
char msg[50]; // for publish // use String object and reserve in setup instead?

// distance sensor (HC-SR04) declarations: ------------------------------------------
const int TRIGGER = D1; 
const int ECHO = D2;
const int MAX_DISTANCE = 200;
NewPing sonar(TRIGGER, ECHO, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
long distance;


// prototypes
void setup_wifi();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void sendReading();
void handleRoot();
void handleNotFound();
void handleStatus();
void handleReset();
void handleLastReadingRequest();
boolean reconnect();
long getDistance();

//globals
long lastReconnectAttempt = 0;
String msgString; // analogue to msg[50] - reserve 50 bytes in setup

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);

  msgString.reserve(50);

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
  server.on("/last", handleLastReadingRequest);
  server.on("/reset", handleReset);
  server.begin();
  Serial.println("HTTP server started");  

  client.setServer(mqtt_server, 1883);  // mqtt server (not http)
  client.setCallback(mqttCallback);         // handles incoming messages


}  // ----------------------- end setup() ---------------------------------

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // all of the subs here have to be repeated in reconnect()
  if (strcmp(topic, "garden/tank-monitor/led/PUT") == 0) { 
    Serial.println("led command received");

     // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1') {
      digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
                                        // but actually the LED is on; this is because
                                        // it is active low on the ESP-01)
    } else {
      digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    }
  }


}  // ------------------------- end callback() ----------------------------

boolean reconnect() { // repeat all of the subs from mqttCallback
  
  if (client.connect("ESP8266Client")) {
    // Once connected, publish an announcement...
    client.publish("garden/tank-monitor/status","tank-monitor connected");
    // ... and resubscribe (all of the same subs as in callback()
    client.subscribe("garden/tank-monitor/led/PUT");
    client.subscribe("garden/tank-monitor/GET");
    client.subscribe("garden/tank-monitor/PUT");
    client.subscribe("garden/census/ping");
  }
  return client.connected();
  
}  // ----------------------------- end reconnect() ------------------------

// ********************************** LOOP *******************************************
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
    client.loop(); //called regularly to allow the client to process incoming messages and maintain its connection to the server
  } 

  // webserver - for checking status and (to do) configuration
  server.handleClient();

  int uS = sonar.ping_median(5);
  distance = sonar.convert_cm(uS); //getDistance(); 
  snprintf (msg, 75, "%ld", distance);
  msgString = String(msg);
  client.publish("garden/tank-monitor/water-level",msg);
  //Serial.print("publishing: "); Serial.println(msg);
  delay(1000); // temporary - move interval checking to updateSensors()
  //updateSensors(); // check distance every 1? second

} // ---------- end loop() -----------------
// ***************************************************************************



