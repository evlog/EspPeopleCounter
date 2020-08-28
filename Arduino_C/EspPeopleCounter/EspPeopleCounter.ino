// Define libraries
// -----
#include <IotWebConf.h> // Library to handle WiFi AP configuration portal
#include <PubSubClient.h> // Library for MQTT
#include <Wire.h>
#include "src/vl53l1x-st-api/vl53l1_api.h"
#include "globals.h"
// -----
// -----

boolean isValidNumber(String str){
   for(byte i=0;i<str.length();i++)
   {
      if(isDigit(str.charAt(i))) return true;
        }
   return false;
} 

// vl53l1 configruation and variables
void vl531Init() {
  
  uint8_t byteData;
  uint16_t wordData;

  uint8_t zone = 0;

  // This is the default 8-bit slave address (including R/W as the least
  // significant bit) as expected by the API. Note that the Arduino Wire library
  // uses a 7-bit address without the R/W bit instead (0x29 or 0b0101001).
  Dev->I2cDevAddr = 0x52;

  VL53L1_software_reset(Dev);

  if (zone == 0) {
    roiConfig.TopLeftX = 4;
    roiConfig.TopLeftY = 11;
    roiConfig.BotRightX = 11;
    roiConfig.BotRightY = 4;
  }
  else if (zone == 1) {
    roiConfig.TopLeftX = 4;
    roiConfig.TopLeftY = 11;
    roiConfig.BotRightX = 11;
    roiConfig.BotRightY = 4;
  }

  VL53L1_RdByte(Dev, 0x010F, &byteData);
  Serial.print(F("VL53L1X Model_ID: "));
  Serial.println(byteData, HEX);
  VL53L1_RdByte(Dev, 0x0110, &byteData);
  Serial.print(F("VL53L1X Module_Type: "));
  Serial.println(byteData, HEX);
  VL53L1_RdWord(Dev, 0x010F, &wordData);
  Serial.print(F("VL53L1X: "));
  Serial.println(wordData, HEX);

  Serial.println(F("Autonomous Ranging Test"));
  status = VL53L1_WaitDeviceBooted(Dev);
  status = VL53L1_DataInit(Dev);
  status = VL53L1_StaticInit(Dev);
  status = VL53L1_SetDistanceMode(Dev, VL53L1_DISTANCEMODE_LONG);
  status = VL53L1_SetMeasurementTimingBudgetMicroSeconds(Dev, MEASUREMENT_BUDGET_MS * 1000);
  status = VL53L1_SetInterMeasurementPeriodMilliSeconds(Dev, INTER_MEASUREMENT_PERIOD_MS);
  status = VL53L1_SetUserROI(Dev, &roiConfig);
  status = VL53L1_StartMeasurement(Dev);

  if(status)
  {
    Serial.println(F("VL53L1_StartMeasurement failed"));
    //while(1);
  }
}

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
  else if (topic_str == mqttPeopleResetTopic) {
    if (message == "true") {
      peopleCounter = 0;
      client.publish(mqttPeopleResetTopic, "OK");
      if (DEBUG) Serial.println("mqttPeopleResetTopic -> true");
    }
  }
  else if (topic_str == mqttSensorRebootTopic) {
    if (message == "true") {
      client.publish(mqttSensorRebootTopic, "OK");
      if (DEBUG) Serial.println("mqttSensorRebootTopic -> true");
      delay(1000);
      ESP.reset();
    }
  }
  else if (topic_str == mqttMeasurementBudgetTopic) {
    if (isValidNumber(message)) {
      MEASUREMENT_BUDGET_MS = message.toInt();
      // Initialize sensor with new congig. value 
      vl531Init(); 
      client.publish(mqttMeasurementBudgetTopic, "OK");
      if (DEBUG) { 
        Serial.print("mqttMeasurementBudgetTopic -> ");
        Serial.println(MEASUREMENT_BUDGET_MS);
      }
    }
  }
  else if (topic_str == mqttMeasurementPeriodTopic) {
    if (isValidNumber(message)) {
      INTER_MEASUREMENT_PERIOD_MS = message.toInt();
      // Initialize sensor with new congig. value 
      vl531Init();    
      client.publish(mqttMeasurementBudgetTopic, "OK");
      if (DEBUG) { 
        Serial.print("mqttMeasurementPeriodTopic -> ");
        Serial.println(INTER_MEASUREMENT_PERIOD_MS);
      }
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
    Serial.println(mqttPeopleResetTopic);
    client.subscribe(mqttPeopleResetTopic);
    Serial.println(mqttSensorRebootTopic);
    client.subscribe(mqttSensorRebootTopic);
    Serial.println(mqttMeasurementBudgetTopic);
    client.subscribe(mqttMeasurementBudgetTopic);
    Serial.println(mqttMeasurementPeriodTopic);
    client.subscribe(mqttMeasurementPeriodTopic);  
    Serial.println(mqttDistanceMeasurementTopic);
    client.subscribe(mqttDistanceMeasurementTopic);  
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

// Check sensor status and get measurements
VL53L1_RangingMeasurementData_t checkGetRangingData() {
  static uint16_t startMs = millis();
  uint8_t isReady;
  static VL53L1_RangingMeasurementData_t rangingData;
  
  // non-blocking for sensor data
  status = VL53L1_GetMeasurementDataReady(Dev, &isReady);

  if(!status)
  {
    if(isReady)
    {
      rangingData = printRangingData();
      VL53L1_ClearInterruptAndStartMeasurement(Dev);
      startMs = millis();
    }
    else if((uint16_t)(millis() - startMs) > VL53L1_RANGE_COMPLETION_POLLING_TIMEOUT_MS)
    {
      Serial.print(F("Timeout waiting for data ready."));
      VL53L1_ClearInterruptAndStartMeasurement(Dev);
      startMs = millis();
    }
  }
  else
  {
    Serial.print(F("Error getting data ready: "));
    Serial.println(status);
  }

  // Optional polling delay; should be smaller than INTER_MEASUREMENT_PERIOD_MS,
  // and MUST be smaller than VL53L1_RANGE_COMPLETION_POLLING_TIMEOUT_MS
  delay(10);

  return rangingData;
}

// Get ranging data from sensor
VL53L1_RangingMeasurementData_t printRangingData() {
  static VL53L1_RangingMeasurementData_t RangingData;

  status = VL53L1_GetRangingMeasurementData(Dev, &RangingData);
  if(!status)
  {
    Serial.print(RangingData.RangeStatus);
    Serial.print(F(","));
    Serial.print(RangingData.RangeMilliMeter);
    Serial.print(F(","));
    Serial.print(RangingData.SignalRateRtnMegaCps/65536.0);
    Serial.print(F(","));
    Serial.println(RangingData.AmbientRateRtnMegaCps/65336.0); 
  }

  return RangingData; 
}

void setup() {
  // put your setup code here, to run once:
   Serial.begin (115200);  
   Serial.println("Serial comm established");
   Serial.print("Sensor MAC address: ");
   Serial.println(MAC_ADDRESS);

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
  sprintf(mqttPeopleResetTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_PEOPLE_RESET_TOPIC);
  sprintf(mqttSensorRebootTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_SENSOR_REBOOT_TOPIC);
  sprintf(mqttMeasurementBudgetTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_MEASUREMENT_BUDGET_TOPIC);
  sprintf(mqttMeasurementPeriodTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_MEASUREMENT_PERIOD_TOPIC);

  if (DEBUG) Serial.print("Wait for MQTT broker...");

  // Subscribe to topics and reconnect to MQTT server
  mqttReconnect();
  //------
  //------

  // vl53l1 configruation and variables
  // -----
  uint8_t byteData;
  uint16_t wordData;

  Wire.begin();
  Wire.setClock(400000);

  // Initialize sensor 
  vl531Init();
}



void loop() {
  // put your main code here, to run repeatedly:



  // If reset connection is lost reset after 20sec.
  //------
  if (WiFi.status() != WL_CONNECTED) {
    delay(20000);

    if (WiFi.status() != WL_CONNECTED)
      ESP.reset();
  }
  //------
  //------
  
  // Reconnect to MQTT broker if not connected
  //------
  if (!client.connected()) {
    mqttReconnect();
  }
  //------
  //------



  // Keep MQTT connection active
  client.loop();


}
