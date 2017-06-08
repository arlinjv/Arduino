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


boolean reconnect() { // code based on mqtt_reconnect_nonblocking example
  
  //if (client.connect("ESP8266Client","sensors/portable-1",1,true,"offline")) { // (clientID, willTopic, willQoS, willRetain, willMessage) - will send 'offline' message when disconnected
  if (client.connect("ESP8266Client","sensors/portable-1",1,true,vBatStr() )) { // (clientID, willTopic, willQoS, willRetain, willMessage) - will send last battery reading
    // Once connected, publish an announcement... (with retention true)
    client.publish("sensors/portable-1","online", true); // retained - any client subscribing to this device will get the online message unless disconnected (see lwt above)
    // ... and resubscribe (all of the same subs as in mqttCallback()
    // be careful not to publish back to subscribe channel
    client.subscribe("sensors/portable-1/led/PUT");
    client.subscribe("sensors/portable-1/GET");               // returns 'online' (unless off-line in which case lwt will give 'offline')
    client.subscribe("sensors/portable-1/status/GET");        // respond by publishing time since last activation at sensors/portable-1/status
    client.subscribe("sensors/census/ping");                  // reply on sensors/census
    client.subscribe("sensors/portable-1/battery/GET");
    // add: reset, 
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

  // Maybe: payload[length] = 0; <-- alternate method from Mark Rose
  // Then: String payloadString((char *) payload);
  Serial.print("mqtt message arrived [");Serial.print(topic);Serial.print("] ");
  printStringWithLength(payload, length);
  Serial.println();

  // all of the subs here have to be registered in reconnect()

  if (strcmp(topic, "sensors/portable-1/status/GET") == 0) {
    //ip address, uptime, time since last, resources --> pack into json dict
    client.publish("sensors/portable-1/status", "nunya",true); 
  }

  if (strcmp(topic, "sensors/portable-1/GET") == 0) {
    client.publish("sensors/portable-1", "online",true); 
  }

  if (strcmp(topic, "sensors/census/ping") == 0)  {
    client.publish("sensors/census","sensors/portable-1");  // sensors/portable-1 is the root so-to-speak for this device. subscribing to sensors/portable-1/# should catch all status changes
  }
  
  if (strcmp(topic, "sensors/portable-1/battery/GET") == 0) {
    //char bLevel [50];
    //sprintf(bLevel, "%d",batteryLevel()); // <-- ask Mark about this: what happened to original buffer - how is this different from dynamic array 
    //client.publish("sensors/portable-1/battery", bLevel);

    /*char bVstr [10];
    //apparently arduino for esp8266 version of sprintf can't handle floats
    dtostrf(vBat(),5, 2, bVstr);     
    //String bVstr = String(batteryLevel(),2);
    //bVstr.toCharArray(bV,6);
    client.publish("sensors/portable-1/battery", bVstr);    
    */
    client.publish("sensors/portable-1/battery",  vBatJson() ); // based on advice found here - http://forum.arduino.cc/index.php?topic=44303.0
  }
  
  if (strcmp(topic, "sensors/portable-1/led/PUT") == 0) { 
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


