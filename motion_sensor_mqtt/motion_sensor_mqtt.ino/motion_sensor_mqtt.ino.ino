/* Basic concept:
 *  - connect to mqtt server
 *  - publish sensor activations to relevant topic
 *  - run webserver for status and configuration purposes.
 *  
 *  pubs:
 *    - sensors/portable-1
 *    - sensors/portable-1/status
 *    - sensors/census
 *    - sensors/portable-1/battery
 *    - 
 *  
 *  subs
 *    - sensors/portable-1/led/PUT
 *    - sensors/portable-1/GET
 *    - sensors/portable-1/status/GET
 *    - sensors/census/ping
 *    - sensors/portable-1/battery/GET
 *    
 * Notes:
 *  - battery sensing code and circuit based on this: https://learn.adafruit.com/using-ifttt-with-adafruit-io?view=all
 *  
 * To do:
 *  - reset after certain number of bad reconnect attempts (maybe incorporate wifi connection test)
 *  - get feedback from Mark regarding battery code 
 *  - (done) figure out why red LED won't stay off - pin is active low - have to set HIGH to shut off
 *  - figure out why lwt battery reading is high
 *  - red LED blink at startup
 *  - (done) switch reconnect to nonblocking method (see pub_sub example)
 *  - use EEPROM (or spi_flash_write and spi_flash_read) to save configuration values
 *  - create webserver interface for updating configuration settings.
 *  - (done) implement a subscription for some sort of broadcast channel (for system wide pings perhaps)
 *  - implement a handler for listing resources (motion, light, temp, etc.)
 *  - have list of subs and pubs. 
 *    - on reconnect loop through subs to subscribe to each in turn.
 *    - ideally create a library to implement different subs
 */

 
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h> // no idea if or why I need this
#include <ESP8266WiFiMulti.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* deviceID = "portable-1";
const char* sensorID = "ms-1"; // may want to drop this

ESP8266WiFiMulti wifiMulti;
ESP8266WebServer server(80);

const char* mqtt_server = "192.168.1.67";
WiFiClient espClient;
PubSubClient client(espClient);
char msg[50]; // for publish

// prototypes
void    setup_wifi();
void    mqttLoop();
void    mqttCallback(char* topic, byte* payload, unsigned int length);
boolean reconnect();
bool    sensorActivated();
void    sendReading();
void    handleRoot();
void    handleNotFound();
void    handleStatus();
void    handleReset();
void    handleLastActivationRequest();

// Hardware declarations: ------------------------------------------
//const int sensorPin = 14; 
const int sensorPin = D7; // frr Amical labelled devices

//globals
unsigned long lastActivation = 0;
long lastReconnectAttempt = 0;

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  digitalWrite(BUILTIN_LED, HIGH);   // this is a portable so shouldn't be wasting battery on LED (active low pin)
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
  client.setCallback(mqttCallback);         // handles incoming messages


}  // ----------------------- end setup() ---------------------------------

void checkADC(){
  static int lastADC = 95;
  static long noChangeCount = 0;
  int adc = analogRead(A0);

  if (abs(adc-lastADC) < 5){
    noChangeCount++;
  }
  else {
    lastADC = adc;
    Serial.print("adc: "); Serial.print(adc); Serial.print("\tNo change count: "); Serial.println(noChangeCount);
    noChangeCount = 0;
  }
    
}


// *****************************************************************************
void loop() {
  delay(1);

  // mqtt - reconnect if necessary otherwise client.loop()
  mqttLoop();
  
  // webserver - for checking status and (to do) configuration
  server.handleClient();

  checkADC();
  
  if (sensorActivated()){
    Serial.println();
    Serial.println("publishing to sensors/portable-1/motion: sensor activated");
    client.publish("sensors/portable-1/motion", "sensor activated");
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
