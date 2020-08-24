// Define libraries
// -----
#include <IotWebConf.h> // Library to handle WiFi AP configuration portal
#include <PubSubClient.h> // Library for MQTT
#include "globals.h"
// -----
// -----

// Function to handle AP WiFi manager configuration page
void handleRoot()
{
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal())
  {
    // -- Captive portal request were already served.
    return;
  }
  String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>IotWebConf 01 Minimal</title></head><body>Hello world!";
  s += "Go to <a href='config'>configure page</a> to change settings.";
  s += "</body></html>\n";

  server.send(200, "text/html", s);
}

// This function is called when an MQTT message is received
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String message(p);
  String topic_str(topic);

  // Print received messages and topics
  //------
  if (DEBUG) Serial.println("");
  if (DEBUG) Serial.println("Callback function");
  if (DEBUG) Serial.println("Topic: ");
  if (DEBUG) Serial.println(topic);

  if (DEBUG) Serial.println("Message: ");
  if (DEBUG) Serial.println(message);
  //------
  //------

  // Check received messages and topics
  //------
  if (topic_str == mqttDebugTopic) {
    if (message == "Hello ESP8266") {
      client.publish(mqttDebugTopic, "Hello server");
      if (DEBUG) Serial.println("MQTT communication established.");
    }
  }

  //------
  //------
}

// Subscribe to available MQTT topics
void topicSubscribe() {
  if(client.connected()) {

    Serial.println("Subscribe to MQTT topics: ");

    Serial.println(mqttDebugTopic);
    client.subscribe(mqttDebugTopic);
    
    client.loop();
  }  
}


// Reconnect to MQTT broker if connection is lost
void mqttReconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    if (DEBUG) Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT, MQTT_USERNAME, MQTT_PASSWORD)) {
      if (DEBUG) Serial.println("MQTT connected");
      topicSubscribe();
    } 
    
    else {
      if (DEBUG) Serial.print("failed, rc=");
      if (DEBUG) Serial.print(client.state());
      if (DEBUG) Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
   Serial.begin (115200);  
   Serial.println("Serial comm established");

  // AP WiFi manager setup
  //---
  boolean validConfig = iotWebConf.init();

  Serial.print("validConfig:");
  Serial.println(validConfig);

  // -- Set up required URL handlers on the web server.
  server.on("/", handleRoot);
  server.on("/config", []{ iotWebConf.handleConfig(); });
  server.onNotFound([](){ iotWebConf.handleNotFound(); }); 
  //---
  //---

  // Check status of WiFi connection and enable the AP WiFi manager page if needed
  // -----
  while(1) {
    iotWebConf.doLoop();
    
    // Try to connect to wifi
    //connect_to_wifi_manager();
    //if (!iotWebConf.checkWifiConnection()) {
    if (WiFi.status() != WL_CONNECTED) {
     Serial.print("Reconnecting to WiFi...\n");
    }
    else {
      Serial.println("WiFi connected\n");
      Serial.println(WiFi.localIP());
      break;
      delay(1000);
    }
  }
  // -----
  // -----  

  // Connect to MQTT broker and subscribe to topics
  //------
  client.setCallback(mqttCallback);

  // Define MQTT topic names
  sprintf(mqttDebugTopic, "%s", MQTT_DEBUG_TOPIC);

  if (DEBUG) Serial.print("Wait for MQTT broker...");

  // Subscribe to topics and reconnect to MQTT server
  mqttReconnect();
  //------
  //------
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Publish");
  client.publish(mqttDebugTopic, "Hello server");
  delay(5000);


}
