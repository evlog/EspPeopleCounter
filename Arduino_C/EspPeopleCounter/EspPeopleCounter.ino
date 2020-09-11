// Define libraries
// -----
//#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> // Library to handle WiFi AP configuration portal 
#include <PubSubClient.h> // Library for MQTT
#include <Wire.h>
#include <StringSplitter.h>
//#include "src/vl53l1x-st-api/vl53l1_api.h"
#include <SparkFun_VL53L1X.h>
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
uint16_t vl531Init() {  
  
  uint16_t distance;
  
  SFEVL53L1X distanceSensor(Wire);

  distanceSensor.setROI(ROI_height, ROI_width, center[Zone]);  // first value: height of the zone, second value: width of the zone
  delay(50);
  distanceSensor.setTimingBudgetInMs(50);
  distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
  distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
  distanceSensor.stopRanging();

  return distance;
  
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
  else if (topic_str == mqttPeopleCountThresholdTopic) {
    if (isValidNumber(message)) {
      PEOPLE_COUNT_THRESHOLD_MM = message.toInt();
      // Initialize sensor with new congig. value 
      //vl531Init(1); 
      client.publish(mqttPeopleCountThresholdTopic, "OK");
      if (DEBUG) { 
        Serial.print("mqttPeopleCountThresholdTopic -> ");
        Serial.println(PEOPLE_COUNT_THRESHOLD_MM);
      }
    }
  }
  else if (topic_str == mqttSensorRebootTopic) {
    if (message == "true") {
      client.publish(mqttSensorRebootTopic, "OK");
      if (DEBUG) Serial.println("mqttSensorRebootTopic -> true");
      delay(1000);
      ESP.restart();
    }
  }
  else if (topic_str == mqttMeasurementBudgetTopic) {
    if (isValidNumber(message)) {
      MEASUREMENT_BUDGET_MS = message.toInt();
      // Initialize sensor with new congig. value 
      //vl531Init(1); 
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
      //vl531Init(1);    
      client.publish(mqttMeasurementPeriodTopic, "OK");
      if (DEBUG) { 
        Serial.print("mqttMeasurementPeriodTopic -> ");
        Serial.println(INTER_MEASUREMENT_PERIOD_MS);
      }
    }
  }
  else if (topic_str == mqttRoiConfig1Topic) {
    if (message.length() > 4) {
      String roiConfig1 = message;
      String p0_1, p1_1, p2_1, p3_1;
      StringSplitter *splitter1 = new StringSplitter(roiConfig1, ',', 4);

      //Read config1 ROI parameters
      p0_1 = splitter1->getItemAtIndex(0);
      p1_1 = splitter1->getItemAtIndex(1);
      p2_1 = splitter1->getItemAtIndex(2);
      p3_1 = splitter1->getItemAtIndex(3); 
      config1TopLeftX = p0_1.toInt();
      config1TopLeftY = p1_1.toInt();
      config1BottomRightX = p2_1.toInt();
      config1BottomRightY = p3_1.toInt(); 
      Serial.println(config1TopLeftX);  
      Serial.println(config1TopLeftY);
      Serial.println(config1BottomRightX);
      Serial.println(config1BottomRightY);
      //vl531Init(1); // Initialize sensor for zone 1
      client.publish(mqttRoiConfig1Topic, "OK");  
    }
  }
  else if (topic_str == mqttRoiConfig2Topic) {
    if (message.length() > 4) {
      String roiConfig2 = message;
      String p0_2, p1_2, p2_2, p3_2;
      StringSplitter *splitter2 = new StringSplitter(roiConfig2, ',', 4);

      //Read config1 ROI parameters
      p0_2 = splitter2->getItemAtIndex(0);
      p1_2 = splitter2->getItemAtIndex(1);
      p2_2 = splitter2->getItemAtIndex(2);
      p3_2 = splitter2->getItemAtIndex(3); 
      config2TopLeftX = p0_2.toInt();
      config2TopLeftY = p1_2.toInt();
      config2BottomRightX = p2_2.toInt();
      config2BottomRightY = p3_2.toInt(); 
      Serial.println(config2TopLeftX);  
      Serial.println(config2TopLeftY);
      Serial.println(config2BottomRightX);
      Serial.println(config2BottomRightY);
      //vl531Init(2); // Initialize sensor for zone 2
      client.publish(mqttRoiConfig2Topic, "OK");  
    }
  }
  else if (topic_str == mqttDistanceModeTopic) {
    if (message.length() > 3) {
      if (message == "short") {
        VL53L1_DISTANCE_MODE = "short";
        client.publish(mqttDistanceModeTopic, "OK");
      }
      else if (message == "long") {     
        VL53L1_DISTANCE_MODE = "long";
        client.publish(mqttDistanceModeTopic, "OK");
      }      
      // Initialize sensor with new congig. value 
      //vl531Init(1);    
      
      if (DEBUG) { 
        Serial.print("mqttDistanceModeTopic -> ");
        Serial.println(VL53L1_DISTANCE_MODE);
      }
    }
  }
  else if (topic_str == mqttRangingPeriodTopic) {
    if (isValidNumber(message)) {
      RANGING_PERIOD_MS = message.toInt();   
      client.publish(mqttRangingPeriodTopic, "OK");
      if (DEBUG) { 
        Serial.print("mqttRangingPeriodTopic -> ");
        Serial.println(RANGING_PERIOD_MS);
      }
    }
  }  
  /*else if (topic_str == mqttDistance1MeasurementTopic) {
    if (isValidNumber(message)) {
      INTER_MEASUREMENT_PERIOD_MS = message.toInt();
      // Initialize sensor with new congig. value 
      vl531Init(1);    
      client.publish(mqttMeasurementBudgetTopic, "OK");
      if (DEBUG) { 
        Serial.print("mqttMeasurementPeriodTopic -> ");
        Serial.println(INTER_MEASUREMENT_PERIOD_MS);
      }
    }
  }*/
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
    Serial.println(mqttPeopleCountThresholdTopic);
    client.subscribe(mqttPeopleCountThresholdTopic); 
    Serial.println(mqttPeopleCountTopic);
    client.subscribe(mqttPeopleCountTopic); 
    Serial.println(mqttSensorRebootTopic);    
    client.subscribe(mqttSensorRebootTopic);
    Serial.println(mqttMeasurementBudgetTopic);
    client.subscribe(mqttMeasurementBudgetTopic);
    Serial.println(mqttMeasurementPeriodTopic);
    client.subscribe(mqttMeasurementPeriodTopic);
    Serial.println(mqttRoiConfig1Topic); 
    client.subscribe(mqttRoiConfig1Topic);    
    Serial.println(mqttRoiConfig2Topic); 
    client.subscribe(mqttRoiConfig2Topic);  
    Serial.println(mqttDistance1MeasurementTopic); 
    client.subscribe(mqttDistance1MeasurementTopic);  
    Serial.println(mqttDistance2MeasurementTopic); 
    client.subscribe(mqttDistance2MeasurementTopic); 
    Serial.println(mqttDistanceModeTopic); 
    client.subscribe(mqttDistanceModeTopic); 
    Serial.println(mqttRangingPeriodTopic); 
    client.subscribe(mqttRangingPeriodTopic); 
    client.loop();
  }  
}


// Reconnect to MQTT broker if connection is lost
void mqttReconnect() {
  // Loop until we're reconnected
  uint8_t counter = 0;
  
  while (!client.connected()) {
    counter++;
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
    if (counter == 5) { // reboot and reconnectr to wifi if MQTT connection is not possible
      ESP.restart();
    }
  }
}

uint16_t ProcessPeopleCountingData(int16_t Distance, uint8_t zone) {

    int CurrentZoneStatus = NOBODY;
    int AllZonesCurrentStatus = 0;
    int AnEventHasOccured = 0;
    uint16_t peopleCounterLocal = 0;

  if (Distance < DIST_THRESHOLD_MAX[Zone]) {
    // Someone is in !
    CurrentZoneStatus = SOMEONE;
  }

  // left zone
  if (zone == LEFT) {

    if (CurrentZoneStatus != LeftPreviousStatus) {
      // event in left zone has occured
      AnEventHasOccured = 1;

      if (CurrentZoneStatus == SOMEONE) {
        AllZonesCurrentStatus += 1;
      }
      // need to check right zone as well ...
      if (RightPreviousStatus == SOMEONE) {
        // event in left zone has occured
        AllZonesCurrentStatus += 2;
      }
      // remember for next time
      LeftPreviousStatus = CurrentZoneStatus;
    }
  }
  // right zone
  else {

    if (CurrentZoneStatus != RightPreviousStatus) {

      // event in left zone has occured
      AnEventHasOccured = 1;
      if (CurrentZoneStatus == SOMEONE) {
        AllZonesCurrentStatus += 2;
      }
      // need to left right zone as well ...
      if (LeftPreviousStatus == SOMEONE) {
        // event in left zone has occured
        AllZonesCurrentStatus += 1;
      }
      // remember for next time
      RightPreviousStatus = CurrentZoneStatus;
    }
  }

  // if an event has occured
  if (AnEventHasOccured) {
    if (PathTrackFillingSize < 4) {
      PathTrackFillingSize ++;
    }

    // if nobody anywhere lets check if an exit or entry has happened
    if ((LeftPreviousStatus == NOBODY) && (RightPreviousStatus == NOBODY)) {

      // check exit or entry only if PathTrackFillingSize is 4 (for example 0 1 3 2) and last event is 0 (nobobdy anywhere)
      if (PathTrackFillingSize == 4) {
        // check exit or entry. no need to check PathTrack[0] == 0 , it is always the case
        Serial.println();
        if ((PathTrack[1] == 1)  && (PathTrack[2] == 3) && (PathTrack[3] == 2)) {
          // This an entry
          peopleCounter ++;
          Serial.print("One person has entered in the room. People in the room now: ");
          Serial.print(peopleCounter);
        } else if ((PathTrack[1] == 2)  && (PathTrack[2] == 3) && (PathTrack[3] == 1)) {
          // This an exit
          peopleCounter --;
          Serial.print("One person has exited the room. People in the room now: ");
          Serial.print(peopleCounter);  
          }
      }
      for (int i=0; i<4; i++){
        PathTrack[i] = 0;
      }
      PathTrackFillingSize = 1;
    }
    else {
      // update PathTrack
      // example of PathTrack update
      // 0
      // 0 1
      // 0 1 3
      // 0 1 3 1
      // 0 1 3 3
      // 0 1 3 2 ==> if next is 0 : check if exit
      PathTrack[PathTrackFillingSize-1] = AllZonesCurrentStatus;
    }
  }

  // output debug data to main host machine
  return(peopleCounter);     
}




void setup() {
  // put your setup code here, to run once:
   Serial.begin (115200);  
   Serial.println("Serial comm established");
   Serial.print("Sensor MAC address: ");
   Serial.println(MAC_ADDRESS);

  // AP WiFi manager setup
  //---
  wifiManager.autoConnect("WifiManager"); // Initial name of the Thing. Used e.g. as SSID of the own Access Point.
  //---
  //---

  // Check status of WiFi connection and enable the AP WiFi manager page if needed
  // -----
  while(1) {    
    // Try to connect to wifi
    //connect_to_wifi_manager();
    //if (!iotWebConf.checkWifiConnection()) {
    if (WiFi.status() != WL_CONNECTED) {
     Serial.print("Reconnecting to WiFi...\n");
     delay(1000);
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
  sprintf(mqttPeopleCountThresholdTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_PEOPLE_COUNT_THRESHOLD_TOPIC);
  sprintf(mqttPeopleCountTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_PEOPLE_COUNT_TOPIC);
  sprintf(mqttSensorRebootTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_SENSOR_REBOOT_TOPIC);
  sprintf(mqttMeasurementBudgetTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_MEASUREMENT_BUDGET_TOPIC);
  sprintf(mqttMeasurementPeriodTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_MEASUREMENT_PERIOD_TOPIC);
  sprintf(mqttRoiConfig1Topic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_ROI_CONFIG1_TOPIC); 
  sprintf(mqttRoiConfig2Topic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_ROI_CONFIG2_TOPIC);
  sprintf(mqttDistance1MeasurementTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_DISTANCE1_MEASUREMENT_TOPIC);
  sprintf(mqttDistance2MeasurementTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_DISTANCE2_MEASUREMENT_TOPIC);
  sprintf(mqttDistanceModeTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_DISTANCE_MODE_TOPIC);
  sprintf(mqttRangingPeriodTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_RANGING_PERIOD_TOPIC);

  if (DEBUG) Serial.print("Wait for MQTT broker...");

  // Subscribe to topics and reconnect to MQTT server
  mqttReconnect();
  //------
  //------



  Wire.begin(); // Define here I2C pins, e.g. Wire.begin(3,4);
  Wire.setClock(400000);


}

void loop() {
  // put your main code here, to run repeatedly:
  bool distance1Flag = false;
  bool distance2Flag = false;
  char temp[50];
  String temp_str;
  unsigned long currentMillis = 0;
  uint16_t RangingData;


  // If reset connection is lost reset after 20sec.
  //------
  if (WiFi.status() != WL_CONNECTED) {
    delay(20000);

    if (WiFi.status() != WL_CONNECTED)
      ESP.restart();
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

  currentMillis = millis();
  // Check and publish the distance measurement 
  //------
  if ((currentMillis - measPreviousMillisRanging) >=  RANGING_PERIOD_MS) {
 
    RangingData = vl531Init(); 

   // if (RangingData != 0) { // Check if we got meaningful distance data
    distance1Flag = true;
      
    temp_str = String(RangingData); //converting ftemp (the float variable above) to a string

    // Add timestamp to distance measurement
    temp_str.concat(',');
    temp_str.concat(String(millis()));
      
    temp_str.toCharArray(temp, temp_str.length() + 1); //packaging up the data to publish to mqtt whoa...

    client.publish(mqttDistance1MeasurementTopic, temp);  
  //  }
    Serial.print("MQTT report, distance: ");
    Serial.println(RangingData);

    measPreviousMillisRanging =  millis();
  }

  //------
  //------

  // inject the new ranged distance in the people counting algorithm
  //------
  delay(200);
  RangingData = vl531Init();
  
  peopleCounterVar = ProcessPeopleCountingData(RangingData, zone);

  zone++;
  zone = zone%2;

  Serial.print("Distance: ");
  Serial.println(RangingData);

  Serial.print("People counter: ");
  Serial.println(peopleCounterVar);
  //------
  //------

  currentMillis = millis();
  // Check and publish the pleople counter value
  //------
  if ((currentMillis - measPreviousMillisPeople) >=  PEOPLE_COUNTER_PERIOD_MS) {
  //  if (distance1Flag) { // Check if we got meaningful distance data for both zone 1 and 2 and increase people counter

      
      
    temp_str = String(peopleCounterVar); //converting ftemp (the float variable above) to a string

    // Add timestamp to distance measurement
    temp_str.concat(',');
    temp_str.concat(String(millis()));     

    temp_str.toCharArray(temp, temp_str.length() + 1); //packaging up the data to publish to mqtt whoa..

    client.publish(mqttPeopleCountTopic, temp);

    Serial.print("MQTT report, people counter: ");
    Serial.println(peopleCounterVar);
 //   }

    measPreviousMillisPeople = millis();
  }

  // Keep MQTT connection active
  client.loop();


}
