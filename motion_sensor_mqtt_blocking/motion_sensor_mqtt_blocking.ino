/* Basic concept:
 *  - 
 *  
 *  
 * Notes:
 *  - pubsub library:
 *    - sketch will reconnect to the server if the connection is lost using a blocking
 *     reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to achieve the same result without blocking the main loop.
 *  - 
 *  
 * To do:
 *  - switch reconnect to nonblocking method (see pub_sub example)
 *  - use EEPROM (or spi_flash_write and spi_flash_read) to save configuration values
 *  - create webserver interface for updating configuration settings.
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

//globals
unsigned long lastActivation = 0;


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

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
}  // ------------------------- end callback() ----------------------------

void reconnect() {
  // Loop until we're reconnected - this blocks may have to use nonblocking method if want to configure via webserver
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("arlin/fromEsp", "hello world");
      // ... and resubscribe
      client.subscribe("arlin/toEsp");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}  // ----------------------------- end reconnect() ------------------------

// *****************************************************************************
void loop() {
  delay(1);

  // mqtt:
  if (!client.connected()) {
    reconnect();
  }
  client.loop();  

  // webserver - for checking status and (to do) configuration
  server.handleClient();
  
  if (sensorActivated()){
    Serial.println();
    Serial.println("publish message: sensor activated");
    client.publish("arlin/fromEsp", "sensor activated");
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
