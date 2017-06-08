void mqttLoop() {
    if (!client.connected()) {
    long now = millis();
    if (now - lastReconnectAttempt > 1000) {
      lastReconnectAttempt = now;
      Serial.println("attempting to connect mqtt");
      if (reconnect()) {// Attempt to reconnect
        Serial.print("mqtt connected - ");Serial.println(mqtt_server);
        lastReconnectAttempt = 0;
      }
    }
  } else {// Client connected
    client.loop(); //called regularly to allow the mqtt client to process incoming messages and maintain its connection to the server
  } 
}  // ----------------------------- end mqttLoop() ------------------------

boolean reconnect() { 
  
  if (client.connect("ESP8266Client","garden/master-irrigation",1,true,"{\"status\":\"offline\",\"url\":\"--\"}")) { // (clientID, willTopic, willQoS, willRetain, willMessage) - will send 'offline' message when disconnected
    // Once connected, publish an announcement...
    client.publish("garden/master-irrigation","online", true); // retained - any client subscribing to this device will get the online message unless disconnected (see will above)
    // ... and resubscribe (all of the same subs as in callback()
    client.subscribe("garden/master-irrigation/led/PUT");
    client.subscribe("garden/master-irrigation/GET");               // returns 'online' (unless off-line in which case lwt will give 'offline')
    client.subscribe("garden/master-irrigation/status/GET");        // respond by publishing detailed status at garden/master-irrigation/status
    client.subscribe("garden/master-irrigation/relay/PUT");
    client.subscribe("garden/master-irrigation/relay/GET");         // reply on garden/master-irrigation/relay ("on" or "off") <-- could get rid of this by just sending retained messages on state change
    client.subscribe("garden/census/ping");                         // reply on garden/census
  }
  return client.connected();
  
}  // ----------------------------- end reconnect() ------------------------


int payloadMatch(const byte *payload, int payloadLength, const char *s) {
  int desiredLength = strlen(s);
  return payloadLength==desiredLength && strncmp((const char *) payload, s, desiredLength)==0;
}

void printStringWithLength(const byte *s, int length) {
  for (int i=0; i < length; ++i) {
    Serial.print((char)s[i]);
  }
}
void mqttCallback(char* topic, byte* payload, unsigned int length) {

  // Maybe: payload[length] = 0;
  // Then: String payloadString((char *) payload);
  Serial.print("mqtt message arrived [");Serial.print(topic);Serial.print("] ");
  printStringWithLength(payload, length);
  Serial.println();

  DynamicJsonBuffer jsonBuffer; // use calculator at https://rawgit.com/bblanchon/ArduinoJson/master/scripts/buffer-size-calculator.html . 100 seems safe
  JsonObject& root = jsonBuffer.createObject();

  // all of the subs here have to be registered in reconnect()
  if (strcmp(topic, "garden/master-irrigation/relay/PUT") == 0) {  //http://www.cplusplus.com/reference/cstring/strcmp/
    if (payloadMatch(payload, length, "on")){
      digitalWrite(controlPin, HIGH);
      status = ON;
      client.publish("garden/master-irrigation/relay","on",true); // retain = true <-- this approach probably makes GET handling superfluous
    }
    else if (payloadMatch(payload, length, "off")) {
      digitalWrite(controlPin, LOW);
      status = OFF;
      client.publish("garden/master-irrigation/relay","off",true);
    }    
  }

  if (strcmp(topic, "garden/master-irrigation/relay/GET") == 0) {  // get rid of this? (see comments above regarding retained messages)
    if (status == ON){
      client.publish("garden/master-irrigation/relay","on",true);
    }
    else {
      client.publish("garden/master-irrigation/relay","off",true);
    }
  }

  if (strcmp(topic, "garden/master-irrigation/status/GET") == 0) { // I think I can drop this - using GET on the root address should give status
    //ip address, uptime, relay, resources --> pack into json dict
    client.publish("garden/master-irrigation/status", "nunya",true); 
  }

  if (strcmp(topic, "garden/master-irrigation/GET") == 0) { // example - {"status":"offline","url":"192.168.50.123","resources":["led","relay"]} --> 88 bytes on esp8266
    root["status"] = "online";
    root["url"] = WiFi.localIP().toString();
    JsonArray& resources = root.createNestedArray("resources");
    resources.add("relay");
    resources.add("led");
    char buff[100];
    root.printTo(buff,sizeof(buff));    
    client.publish("garden/master-irrigation", buff,true); 

    Serial.print("publishing: ");Serial.println(buff);
  }

  if (strcmp(topic, "garden/census/ping") == 0)  {
    client.publish("garden/census","garden/master-irrigation");  // garden/master-irrigation is the root so-to-speak for this device. subscribing to garden/master-irrigation/# should catch all status changes
  }
  
  if (strcmp(topic, "garden/master-irrigation/led/PUT") == 0) { 
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

}  // ------------------------- end mqttCallback() ----------------------------


