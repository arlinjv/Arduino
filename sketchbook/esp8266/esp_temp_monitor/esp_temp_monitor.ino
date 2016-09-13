 /*
 *  esp_temp_monitor
 *  
 *  - Status: everything and the kitchen sink right now
 *            - not a finished product - moving on to learn websockets
 *            - good basis for operating as stand alone webserver
 *            - also has foundation for restful API approach
 *  
 *  - based on 4.6.16 version of esp_sensor_node_multi
 *  - implementing for purpose of monitoring chick brooder environment
 *  - DS18B20 support based on tester.ino example in DallasTemperature.h library examples folder
 *  
 *  
 *  Simple webserver and client functionality    
 *  - Webserver and client aspects are both used primarily for sending information  
 *  - Client functionality is in sendReading() and is used to send data by means of HTTP GET
 *  - To test client easily, run SimpleHTTPServer at cl: - python -m SimpleHTTPServer 5000
 *  
 *  Uses ESP8266WiFiMulti.h library to simplify use on various APs
 *  
 *  To do:
 *   - Build server webpage for displaying temperatures
 *      - done - see whether blocking code in thermo routines disrupts webserver - seems ok
 *      - try adding javascript
 *      - add refresh button
 *        - add refresh rate form
 *   - Add handlers for providing temp readings
 *      - via api (e.g. server.on("/temps", handleTempsRequest);
 *      - via request (parsing GET string)
 *    - Have a button to get latest reading
 *      - See AdvancedWebServer example - notice how <img src=\"/test.svg\" />\ invokes handler for /test.svg
 *        - also note use of <meta http-equiv='refresh' content='5'/> to set refresh rate of page
 *      - some ideas here http://stackoverflow.com/questions/2906582/how-to-create-an-html-button-that-acts-like-a-link
 *      - this looks promising, too: http://www.tutorialscollection.com/html-button-link-making-button-as-href-link-examples/
 *      - great looking example for implementing buttons: https://gist.github.com/igrr/3b66292cf8708770f76b
 *  
 *   - Add DH11 support.
 *  
 *  
 *    - Customize callback handlers to provide data in json or url format
 *    - add additional sensors
 *    - Add LED feedback for various situations.
 *      - red - pin 0, blue - pin 2  e.g.: pinMode(0, OUTPUT); (in setup) then digitalWrite(0, HIGH);
 *      - 

 *    - Add checkin code - change settings (like transmit frequency) based on return from server
 *      - maybe have code for a special check in switch so that node will send a check in message to server
 *      - maybe have some way of booting into AP mode for changing configuration settings
 *    
 *  Done:
 *    
 */

#include <ESP8266WiFi.h>
#include <WiFiClient.h> // consider replacing with ESP8266HTTPClient.h - approach seems to be more consistent with ESP8266WebServer methods - see library examples
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h> // no idea why I need this
#include <ESP8266WiFiMulti.h>

#include <OneWire.h>
#include <DallasTemperature.h>

// WiFi related declarations: --------------------------------------------------

const char* host = "192.168.1.67"; //on ATT226
//const char* host = "192.168.50.124"; // on i-GATE
const int httpPort = 5000; // standard Flask port

const char* deviceID = "esp1";
const char* lightSensorID = "ls1";

const int analogInPin = A0; 

const int led = 0;

ESP8266WiFiMulti wifiMulti;

ESP8266WebServer server(80);

WiFiClient client; // should still need for connecting to another website

// Temp sensor related declarations ---------------------------------------------

#define ONE_WIRE_BUS D2 // 
#define TEMPERATURE_PRECISION 9 // Lower resolution

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

int numberOfDevices; // Number of temperature devices found

//DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address // avoid using global

struct thermo {
  String address; // change this to String
  float tempC;
};

thermo thermos[10]; // won't likely ever have more than ten sensors. 
                    // couldn't figure out how to size dynamically at global level
                    //ability to declare variable length arrays does exist: http://stackoverflow.com/questions/20826881/variable-array-size-at-arduino

// prototype declarations -----------------------------------------------------
// it appears IDE isn't autoprototyping for ESP8266. may already be fixed in new builds - https://github.com/esp8266/Arduino/issues/1066

void    initThermos();
void    printTemperature(DeviceAddress deviceAddress);
String  addressToString(DeviceAddress address);
void    updateTemps();
 
void    handleRoot();
void    handleNotFound();
void    handleDataRequest();
String  getLDRString();
void    sendReading();
void    drawGraph();

void setup() { // ===============================================================
  
  Serial.begin(115200);
  //Serial.setDebugOutput(true); //using this may conflict with libraries that use printf - like in the WiFiClientEvents.ino example
  Serial.println(); // to keep output from getting stuck at end of opening gibberish
  
  initThermos();
  
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

  server.on("/data", handleDataRequest);

  server.on ( "/test.svg", drawGraph ); // change to /graph.svg

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  }); // ah, inline refers to inline method. cool, but what is the array notation for?

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");  

  
} // ----------------------- end setup() ---------------------------------

void loop() {
  delay(1); // drop this?

  server.handleClient();

} // ---------- end loop() -----------------




String getLDRString(){
  int adc = analogRead(analogInPin);

  String dataString = "sensor_ID=ls1&sensor_type=light&sensor_value=";
  dataString += adc;
  dataString += "&units=raw";
  
  return dataString;
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

  //url += "?value=";
  //url += value;
  //url += "&millis=";
  //url += millis(); 

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

