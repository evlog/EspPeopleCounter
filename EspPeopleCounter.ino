// Define libraries
// -----
//#include <DNSServer.h>
extern "C" {
#include "esp_wifi.h"
}

#include <WiFiManager.h> // Library to handle WiFi AP configuration portal 
#include <PubSubClient.h> // Library for MQTT
#include <Wire.h>
#include <StringSplitter.h>
#include <SparkFun_VL53L1X.h>
#include "ESP32HTTPUpdate.h"
#include <EEPROM.h>
#include "globals.h"
// ----- 
// -----




SFEVL53L1X distanceSensor(Wire);


// Function to return random MQTT_CLIENT name
// -----
void randomMqttClientName() {
  String MQTT_CLIENT_NAME;
  Serial.println("Random:");
  randomSeed(ESP.getCycleCount()); 
  MQTT_CLIENT_NAME = "clientName";
  MQTT_CLIENT_NAME.concat(String(random(1,30000)));
  //MQTT_CLIENT_NAME.toCharArray(MQTT_CLIENT, MQTT_CLIENT_NAME.length() + 1);
  MQTT_CLIENT = MQTT_CLIENT_NAME.c_str();
  //sprintf(mqttDebugTopic, "%s", MQTT_DEBUG_TOPIC);
  Serial.print("Client name: ");
  Serial.println(MQTT_CLIENT);
}
// ----- 
// -----

// Function to read EEPROM and initialize config. parameter
// -----
void mqttForceInitConfig() {
  MEASUREMENT_BUDGET_MS = 50;
  intToEeprom(MEASUREMENT_BUDGET_MS, 1);
  INTER_MEASUREMENT_PERIOD_MS = 55;
  intToEeprom(INTER_MEASUREMENT_PERIOD_MS, 7);
  DIST_THRESHOLD_MAX[0] = 850;
  DIST_THRESHOLD_MAX[1] = 850;
  intToEeprom(DIST_THRESHOLD_MAX[0], 13);
  intToEeprom(DIST_THRESHOLD_MAX[1], 19);
  center[0] = 239;
  center[1] = 175;
  intToEeprom(center[0], 25);
  intToEeprom(center[1], 31);
  VL53L1_DISTANCE_MODE = "long";
  if(VL53L1_DISTANCE_MODE == "short")
    intToEeprom(1, 37);
  else if(VL53L1_DISTANCE_MODE == "long")
    intToEeprom(2, 37);
  RANGING_PERIOD_MS = 10000;    
  intToEeprom(RANGING_PERIOD_MS, 43); 
  PEOPLE_COUNTER_PERIOD_MS = 120000;
  intToEeprom(PEOPLE_COUNTER_PERIOD_MS, 49);
  ROI_height = 5;
  intToEeprom(ROI_height, 55);
  ROI_width = 5;
  intToEeprom(ROI_width, 61);
  WIFI_MANAGER_ENABLE = 1; 
  intToEeprom(WIFI_MANAGER_ENABLE, 79);
}
// -----
// -----

// Function to read EEPROM and initialize config. parameter
// -----
void initEepromConfigWrite() {
  intToEeprom(MEASUREMENT_BUDGET_MS, 1);
  intToEeprom(INTER_MEASUREMENT_PERIOD_MS, 7);
  intToEeprom(DIST_THRESHOLD_MAX[0], 13);
  intToEeprom(DIST_THRESHOLD_MAX[1], 19);
  intToEeprom(center[0], 25);
  intToEeprom(center[1], 31);
  if(VL53L1_DISTANCE_MODE == "short")
    intToEeprom(1, 37);
  else if(VL53L1_DISTANCE_MODE == "long")
    intToEeprom(2, 37);    
  intToEeprom(RANGING_PERIOD_MS, 43); 
  Serial.println("**");
  Serial.println(PEOPLE_COUNTER_PERIOD_MS);
  intToEeprom(PEOPLE_COUNTER_PERIOD_MS, 49);
  intToEeprom(ROI_height, 55);
  intToEeprom(ROI_width, 61);
  intToEeprom(WIFI_MANAGER_ENABLE, 79);
}
// -----
// -----

// Function to store 6-digit int value to EEPROM
// -----
void intToEeprom(uint32_t param, int addr) {
  String f;

  f = String(param);

 if (param < 10) {
  EEPROM.write(addr, 0);
  EEPROM.commit();
  EEPROM.write(addr+1, 0);
  EEPROM.commit();
  EEPROM.write(addr+2, 0);
  EEPROM.commit();
  EEPROM.write(addr+3, 0);
  EEPROM.commit();
  EEPROM.write(addr+4, 0);
  EEPROM.commit();
  EEPROM.write(addr+5, f.substring(0,1).toInt());
  EEPROM.commit();
 }
 else if((param >= 10) && (param < 100)) {
  EEPROM.write(addr, 0);
  EEPROM.commit();
  EEPROM.write(addr+1, 0);
  EEPROM.commit();
  EEPROM.write(addr+2, 0);
  EEPROM.commit();
  EEPROM.write(addr+3, 0);
  EEPROM.commit();
  EEPROM.write(addr+4, f.substring(0,1).toInt());
  EEPROM.commit();
  EEPROM.write(addr+5, f.substring(1,2).toInt());
  EEPROM.commit();
 }
 else if((param >= 100) && (param < 1000)) {
  EEPROM.write(addr, 0);
  EEPROM.commit();
  EEPROM.write(addr+1, 0);
  EEPROM.commit();
  EEPROM.write(addr+2, 0);
  EEPROM.commit();
  EEPROM.write(addr+3, f.substring(0,1).toInt());
  EEPROM.commit();
  EEPROM.write(addr+4, f.substring(1,2).toInt());
  EEPROM.commit();
  EEPROM.write(addr+5, f.substring(2,3).toInt());
  EEPROM.commit();
 }
 else if((param >= 1000) && (param < 10000)) {
  EEPROM.write(addr, 0);
  EEPROM.commit();
  EEPROM.write(addr+1, 0);
  EEPROM.commit();
  EEPROM.write(addr+2, f.substring(0,1).toInt());
  EEPROM.commit();
  EEPROM.write(addr+3, f.substring(1,2).toInt());
  EEPROM.commit();
  EEPROM.write(addr+4, f.substring(2,3).toInt());
  EEPROM.commit();
  EEPROM.write(addr+5, f.substring(3,4).toInt());
  EEPROM.commit();
 }
 else if((param >= 10000) && (param < 100000)) {
  EEPROM.write(addr, 0);
  EEPROM.commit();
  EEPROM.write(addr+1, f.substring(0,1).toInt());
  EEPROM.commit();
  EEPROM.write(addr+2, f.substring(1,2).toInt());
  EEPROM.commit();
  EEPROM.write(addr+3, f.substring(2,3).toInt());
  EEPROM.commit();
  EEPROM.write(addr+4, f.substring(3,4).toInt());
  EEPROM.commit();
  EEPROM.write(addr+5, f.substring(4,5).toInt());
  EEPROM.commit();
 }
 else if(param >= 100000) {
  EEPROM.write(addr, f.substring(0,1).toInt());
  EEPROM.commit();
  EEPROM.write(addr+1, f.substring(1,2).toInt());
  EEPROM.commit();
  EEPROM.write(addr+2, f.substring(2,3).toInt());
  EEPROM.commit();
  EEPROM.write(addr+3, f.substring(3,4).toInt());
  EEPROM.commit();
  EEPROM.write(addr+4, f.substring(4,5).toInt());
  EEPROM.commit();
  EEPROM.write(addr+5, f.substring(5,6).toInt());
  EEPROM.commit();
 }
}
// -----
// -----

// Function to read EEPROM and initialize config. parameters
// -----
void restoreEppromConfig() {
  Serial.println("Restore config. parameters from EEPROM");
  MEASUREMENT_BUDGET_MS = EepromToInt(1);
  Serial.print("MEASUREMENT_BUDGET_MS:");
  Serial.println(MEASUREMENT_BUDGET_MS);
  INTER_MEASUREMENT_PERIOD_MS = EepromToInt(7);
  Serial.print("INTER_MEASUREMENT_PERIOD_MS:");
  Serial.println(INTER_MEASUREMENT_PERIOD_MS);
  DIST_THRESHOLD_MAX[0] = EepromToInt(13);
  Serial.print("DIST_THRESHOLD_MAX[0]:");
  Serial.println(DIST_THRESHOLD_MAX[0]);
  DIST_THRESHOLD_MAX[1] = EepromToInt(19);
  Serial.print("DIST_THRESHOLD_MAX[1]:");
  Serial.println(DIST_THRESHOLD_MAX[1]);
  center[0] = EepromToInt(25);
  Serial.print("center[0]:");
  Serial.println(center[0]);
  center[1] = EepromToInt(31);
  Serial.print("center[1]:");
  Serial.println(center[1]);
  if(EepromToInt(37) == 1)
    VL53L1_DISTANCE_MODE = "short";
  if(EepromToInt(37) == 2)
    VL53L1_DISTANCE_MODE = "long";  
  Serial.print("VL53L1_DISTANCE_MODE:");
  Serial.println(VL53L1_DISTANCE_MODE);
  RANGING_PERIOD_MS = EepromToInt(43);
  Serial.print("RANGING_PERIOD_MS:");
  Serial.println(RANGING_PERIOD_MS);
  PEOPLE_COUNTER_PERIOD_MS = EepromToInt(49); 
  Serial.print("PEOPLE_COUNTER_PERIOD_MS:");
  Serial.println(PEOPLE_COUNTER_PERIOD_MS); 
  ROI_height = EepromToInt(55); 
  Serial.print("ROI_height:");
  Serial.println(ROI_height); 
  ROI_width = EepromToInt(61); 
  Serial.print("ROI_width:");
  Serial.println(ROI_width); 
  //SD_DEVIATION_THRESHOLD = EepromToInt(73); 
  //Serial.print("SD_DEVIATION_THRESHOLD:");
  //Serial.println(SD_DEVIATION_THRESHOLD);
  WIFI_MANAGER_ENABLE = EepromToInt(79); 
  Serial.print("WIFI_MANAGER_ENABLE:");
  Serial.println(WIFI_MANAGER_ENABLE);
}
// -----
// -----

// Function to read 6-digit int value from EEPROM
// -----
uint32_t EepromToInt(int addr) {
  uint32_t param = 0;

  param +=  EEPROM.read(addr) * 100000;
  param +=  EEPROM.read(addr+1) * 10000;
  param +=  EEPROM.read(addr+2) * 1000;
  param +=  EEPROM.read(addr+3) * 100;
  param +=  EEPROM.read(addr+4) * 10;
  param +=  EEPROM.read(addr+5) * 1;

  return param;
}
// -----
// -----

// HTTP flash update functions
// -----
void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
} 

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

void flashUpdate() {
  if (WiFi.status() == WL_CONNECTED) {

    //WiFiClient Wclient;  
    
     t_httpUpdate_return ret = ESPhttpUpdate.update(UPDATE_BINARY_FILE_PATH);
    // Or:
   // t_httpUpdate_return ret = ESPhttpUpdate.update(Wclient, "http://192.168.1.40/subs/figures/", 80, "test.bin");

    switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        break;
    }
  }
}
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
uint16_t vl531Init(uint8_t zone) {  
  
  uint16_t distance;
  
  if (distanceSensor.init() == false);

  distanceSensor.setROI(ROI_height, ROI_width, center[zone]);  // first value: height of the zone, second value: width of the zone

  if (VL53L1_DISTANCE_MODE == "short")
    distanceSensor.setDistanceModeShort();
  else if (VL53L1_DISTANCE_MODE == "long")
    distanceSensor.setDistanceModeLong();
    
  //delay(50);
  distanceSensor.setIntermeasurementPeriod(INTER_MEASUREMENT_PERIOD_MS);
  distanceSensor.setTimingBudgetInMs(MEASUREMENT_BUDGET_MS);
  distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
  distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
  distanceSensor.stopRanging();

  //processPeopleCountingData(distance, Zone);
  return distance;
  
}


// This function is called when an MQTT message is received
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String message(p);
  String topic_str(topic);
  char temp[300];
  String temp_str;
  uint32_t tmp_int;

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
    if (message.length() > 2) {
      String countThreshold = message;
      String t0, t1;
      StringSplitter *splitterTh = new StringSplitter(countThreshold, ',', 2);

      //Read threshold
      t0 = splitterTh->getItemAtIndex(0);
      t1 = splitterTh->getItemAtIndex(1);


      if ((t0.toInt() != 0) && (t1.toInt() != 0)) {
        if ((t0.toInt() < 100) | (t0.toInt()) > 4000 | (t1.toInt() < 100) | (t1.toInt() > 4000)) {
          Serial.print(mqttPeopleCountThresholdTopic);
          Serial.println("->ERROR");
          client.publish(mqttPeopleCountThresholdTopic, "ERROR");
          client.publish(mqttDummyTopic, "Ignore this message");
        }
        else {
          Serial.print(mqttPeopleCountThresholdTopic);
          Serial.println("->OK");
          DIST_THRESHOLD_MAX[0] = t0.toInt();
          intToEeprom(DIST_THRESHOLD_MAX[0], 13);
          DIST_THRESHOLD_MAX[1] = t1.toInt();
          intToEeprom(DIST_THRESHOLD_MAX[1], 19);
          client.publish(mqttPeopleCountThresholdTopic, "OK");
        }
      }
      Serial.println(DIST_THRESHOLD_MAX[0]);
      Serial.println(DIST_THRESHOLD_MAX[1]);
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
  else if (topic_str == mqttSensorResetTopic) {
    if (message == "true") {
      client.publish(mqttSensorResetTopic, "OK");
      if (DEBUG) Serial.println("mqttSensorResetTopic -> true");
      delay(1000);
      ESP.restart();
    }
  }
  else if (topic_str == mqttMeasurementBudgetTopic) {
    if (isValidNumber(message)) {
      if ((message.toInt() != 15) && (message.toInt() != 20) && (message.toInt() != 33) && (message.toInt() != 50) && (message.toInt() != 100) && (message.toInt() != 200) && (message.toInt() != 500)) {     
        Serial.print(mqttMeasurementBudgetTopic);
        Serial.println("->ERROR");
        client.publish(mqttMeasurementBudgetTopic, "ERROR");
      }
      else {
        MEASUREMENT_BUDGET_MS = message.toInt();
        intToEeprom(MEASUREMENT_BUDGET_MS, 1);
        Serial.print(mqttMeasurementBudgetTopic);
        Serial.println("->OK");
        client.publish(mqttMeasurementBudgetTopic, "OK");
      }
      if (DEBUG) { 
        Serial.print("mqttMeasurementBudgetTopic -> ");
        Serial.println(MEASUREMENT_BUDGET_MS);
      }
    }
  }
  else if (topic_str == mqttMeasurementPeriodTopic) {
    if (isValidNumber(message)) {
      if (message.toInt() < 20) {
        Serial.print(mqttMeasurementPeriodTopic);
        Serial.println("->ERROR");
        client.publish(mqttMeasurementPeriodTopic, "ERROR");
      }
      else {
        INTER_MEASUREMENT_PERIOD_MS = message.toInt();  
        intToEeprom(INTER_MEASUREMENT_PERIOD_MS, 7);   
        Serial.print(mqttMeasurementPeriodTopic);
        Serial.println("->OK");
        client.publish(mqttMeasurementPeriodTopic, "OK");   
      }

      if (DEBUG) { 
        Serial.print("mqttMeasurementPeriodTopic -> ");
        Serial.println(INTER_MEASUREMENT_PERIOD_MS);
      }
    }
  }
  else if (topic_str == mqttRoiConfigTopic) {
    if (message.length() > 4) {
      String roiConfig = message;
      String p0, p1, p2, p3;
      StringSplitter *splitterRoi = new StringSplitter(roiConfig, ',', 4);

      //Read config1 ROI parameters
      p0 = splitterRoi->getItemAtIndex(0);
      p1 = splitterRoi->getItemAtIndex(1);
      p2 = splitterRoi->getItemAtIndex(2);
      p3 = splitterRoi->getItemAtIndex(3); 
      ROI_height = p0.toInt();
      intToEeprom(ROI_height, 55);
      ROI_width = p1.toInt();
      intToEeprom(ROI_width, 61);
      center[0] = p2.toInt();
      intToEeprom(center[0], 25);
      center[1] = p3.toInt();
      intToEeprom(center[1], 31);
      Serial.println(ROI_height);  
      Serial.println(ROI_width);
      Serial.println(center[0]);
      Serial.println(center[1]);
      //vl531Init(1); // Initialize sensor for zone 1
      client.publish(mqttRoiConfigTopic, "OK");  
    }
  }
  else if (topic_str == mqttDistanceModeTopic) {
    if (message.length() > 3) {
      if (message == "short") {
        VL53L1_DISTANCE_MODE = "short";
        intToEeprom(1, 37);
        client.publish(mqttDistanceModeTopic, "OK");
      }
      else if (message == "long") {     
        VL53L1_DISTANCE_MODE = "long";
        intToEeprom(2, 37);
        client.publish(mqttDistanceModeTopic, "OK");
      }          
      if (DEBUG) { 
        Serial.print("mqttDistanceModeTopic -> ");
        Serial.println(VL53L1_DISTANCE_MODE);
      }
    }
  }
  else if (topic_str == mqttRangingPeriodTopic) {
    if (isValidNumber(message)) {
      if (message.toInt() < 1000) {
        Serial.print(mqttRangingPeriodTopic);
        Serial.println("->ERROR");
      }
      else {
        RANGING_PERIOD_MS = message.toInt();
        intToEeprom(RANGING_PERIOD_MS, 43);
        Serial.print(mqttRangingPeriodTopic);
        Serial.println("->OK");
        client.publish(mqttRangingPeriodTopic, "OK");
      }   
      if (DEBUG) { 
        Serial.print("mqttRangingPeriodTopic -> ");
        Serial.println(RANGING_PERIOD_MS);
      }
    }
  }  
  else if (topic_str == mqttFlashUpdateTopic) {
    if (isValidNumber(message)) {
      if (message.toInt() == 1) {   
        client.publish(mqttFlashUpdateTopic, "OK");
        if (DEBUG) { 
          Serial.print("mqttFlashUpdateTopic -> ");
          Serial.println(message.toInt());
          Serial.println("Flash update triggered via web interface...");
        }
        flashUpdate();
      }
    }
  } 
  else if (topic_str == mqttRestoreSensorConfigTopic) {
    if (isValidNumber(message)) {
      if (message.toInt() == 1) {   
        client.publish(mqttRestoreSensorConfigTopic, "OK");
        if (DEBUG) { 
          Serial.print("mqttRestoreSensorConfigTopic -> ");
          Serial.println(message.toInt());
          Serial.println("Flash update triggered via web interface...");
        }
        mqttForceInitConfig();
      }
    }
  
  }
  else if (topic_str == mqttWifiManagerEnableTopic) {
    if (isValidNumber(message)) {
      if ((message.toInt() == 1) | (message.toInt() == 0)) {   
        client.publish(mqttWifiManagerEnableTopic, "OK");
        if (DEBUG) { 
          Serial.print("mqttWifiManagerEnableTopic -> ");
          Serial.println(message.toInt());
           if (message.toInt() == 1) {
            Serial.println("WiFi manager enabled");
            WIFI_MANAGER_ENABLE = 1;
            intToEeprom(WIFI_MANAGER_ENABLE, 79);
            wifiManager.resetSettings();
            ESP.restart();
           }
           else if (message.toInt() == 0) {
            Serial.println("WiFi manager disabled");     
            WIFI_MANAGER_ENABLE = 0;
            intToEeprom(WIFI_MANAGER_ENABLE, 79);
           }     
        }
      }
    }
  }
  else if (topic_str == mqttGetSensorConfigTopic) {
    if (isValidNumber(message)) {
      if (message.toInt() == 1) {   
        //client.publish(mqttGetSensorConfigTopic, "OK");
        if (DEBUG) { 
          Serial.print("mqttGetSensorConfigTopic -> ");
          Serial.println(message.toInt());
          Serial.println("Sensor configuration parameters:");

          temp_str = "MEASUREMENT_BUDGET_MS: ";
          temp_str.concat(String(MEASUREMENT_BUDGET_MS));
  

          temp_str.concat("|\nINTER_MEASUREMENT_PERIOD_MS: ");
          temp_str.concat(String(INTER_MEASUREMENT_PERIOD_MS));
     

          temp_str.concat("|\nDIST_THRESHOLD_MAX: ");
          temp_str.concat(String(DIST_THRESHOLD_MAX[0]));
          temp_str.concat(',');
          temp_str.concat(String(DIST_THRESHOLD_MAX[1]));
      

          temp_str.concat("|\nROI_CENTER: ");
          temp_str.concat(String(center[0]));
          temp_str.concat(',');
          temp_str.concat(String(center[1]));
     

          temp_str.concat("|\nVL53L1_DISTANCE_MODE: ");
          temp_str.concat(VL53L1_DISTANCE_MODE);
     

          temp_str.concat("|\nRANGING_PERIOD_MS: ");
          temp_str.concat(RANGING_PERIOD_MS);
          

          temp_str.concat("|\nPEOPLE_COUNTER_PERIOD_MS: ");
          temp_str.concat(PEOPLE_COUNTER_PERIOD_MS);
          

          temp_str.concat("|\nWIFI_MANAGER_ENABLE: ");
          temp_str.concat(WIFI_MANAGER_ENABLE);
          temp_str.toCharArray(temp, temp_str.length() + 1);
          client.publish(mqttGetSensorConfigTopic, temp);
          Serial.println(temp_str);
          
        }
      }
    }
  }  
 
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
    Serial.println(mqttSensorResetTopic);    
    client.subscribe(mqttSensorResetTopic);
    Serial.println(mqttMeasurementBudgetTopic);
    client.subscribe(mqttMeasurementBudgetTopic);
    Serial.println(mqttMeasurementPeriodTopic);
    client.subscribe(mqttMeasurementPeriodTopic);
    Serial.println(mqttRoiConfigTopic); 
    client.subscribe(mqttRoiConfigTopic);     
    Serial.println(mqttDistance1MeasurementTopic); 
    client.subscribe(mqttDistance1MeasurementTopic);  
    Serial.println(mqttDistance2MeasurementTopic); 
    client.subscribe(mqttDistance2MeasurementTopic); 
    Serial.println(mqttDistanceModeTopic); 
    client.subscribe(mqttDistanceModeTopic); 
    Serial.println(mqttRangingPeriodTopic); 
    client.subscribe(mqttRangingPeriodTopic); 
    Serial.println(mqttFlashUpdateTopic); 
    client.subscribe(mqttFlashUpdateTopic); 
    Serial.println(mqttGetSensorConfigTopic); 
    client.subscribe(mqttGetSensorConfigTopic); 
    Serial.println(mqttRestoreSensorConfigTopic); 
    client.subscribe(mqttRestoreSensorConfigTopic); 
    Serial.println(mqttWifiManagerEnableTopic); 
    client.subscribe(mqttWifiManagerEnableTopic); 
    //Serial.println(mqttDummyTopic); 
    client.subscribe(mqttDummyTopic); 
    client.loop();
  }  
}


// Reconnect to MQTT broker if connection is lost
void mqttReconnect() {
  // Loop until we're reconnected
  uint8_t counter = 0;

  // Generate random client ID name
  //------
  String MQTT_CLIENT_NAME;
  Serial.println("Random:");
  randomSeed(ESP.getCycleCount()); 
  MQTT_CLIENT_NAME = "clientName";
  MQTT_CLIENT_NAME.concat(String(random(1,30000)));
  //MQTT_CLIENT_NAME.toCharArray(MQTT_CLIENT, MQTT_CLIENT_NAME.length() + 1);
  MQTT_CLIENT = MQTT_CLIENT_NAME.c_str();
  //sprintf(mqttDebugTopic, "%s", MQTT_DEBUG_TOPIC);
  Serial.print("Client name: ");
  Serial.println(MQTT_CLIENT);
  //------
  //------
  
  
  while (!client.connected()) {
    counter++;
    if (DEBUG) Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT, MQTT_USERNAME, MQTT_PASSWORD)) {
      if (DEBUG) {
        Serial.println("MQTT connected");
        Serial.println(MQTT_CLIENT);
      }
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
          // Prevent negative numbers on the counter        
          if (peopleCounter != 0)
            peopleCounter --;
          else
            peopleCounter = 0;
            
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
  int i;
  
  // put your setup code here, to run once:
  Serial.begin (115200);  
  delay(500);
  Serial.println("Serial comm established");
  Serial.print("Sensor MAC address: ");
  WiFi.mode(WIFI_MODE_STA);
 
  MAC_ADDRESS = WiFi.macAddress();
  Serial.println(MAC_ADDRESS);


  Wire.begin(); // Define here I2C pins, e.g. Wire.begin(3,4);
  Wire.setClock(400000);

  // Initialize EPPROM memory
  EEPROM.begin(512);

  //// Initialize measurements array
  //for (i = 0; i < measArrSize; i++)
    //measArr[i] = 0;


  
  //Detect if this is the first boot and initialize in EEPROM the sensor configuration parameters
  if (EEPROM.read(0) != 4) {
    Serial.println("Virgin boot");
    EEPROM.write(eeprom_addr, 4);
    EEPROM.commit();

    initEepromConfigWrite();
  }
  else {
    restoreEppromConfig();
  }

  Serial.println("WIFI_MANAGER_ENABLE:");
  Serial.println(WIFI_MANAGER_ENABLE);


  int x = 1;

  // Try to connect on fixed WiFi SSID and if not start the wifiManager
  if (x == 0) {
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);
    //WiFi.disconnect(true);
    //ESP.eraseConfig();
    delay(2000);

    delay(1000);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
    int wifiCounter = 0;
    Serial.println("Wait for WiFi fixed SSID 11N...");
    //WiFi.setPhyMode(WIFI_PHY_MODE_11N);
  //wifi_set_phy_mode(PHY_MODE_11N);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      wifiCounter++;
      if (wifiCounter == 40) {
        Serial.print("Failed to connect to fixed SSID 11N mode. Restarting."); 
        ESP.restart();
        //break;
      }
    }
    Serial.print("WIFI MODE: ");
    uint8_t protocol_bitmap;
    Serial.println(esp_wifi_get_protocol(ESP_IF_WIFI_STA, &protocol_bitmap));
    Serial.println(protocol_bitmap);
  }
  //---
  //---


  else if (WIFI_MANAGER_ENABLE == 1) {
  // Enable the AP WiFi manager page if needed
  //---
  if (WiFi.status() != WL_CONNECTED) 
    wifiManager.autoConnect("Intuitive_Sensor"); // Initial name of the Thing. Used e.g. as SSID of the own Access Point.
  }
  //---
  //---
  

  // Check status of WiFi connection and 
  // -----
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // -----
 
  // Connect to MQTT broker and subscribe to topics 
  //------
  client.setBufferSize(512); // Set buffer size to 512bytes to handle the getConfig command payload
  client.setCallback(mqttCallback);

  // Define MQTT topic names
  sprintf(mqttDebugTopic, "%s", MQTT_DEBUG_TOPIC);
  sprintf(mqttPeopleResetTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_PEOPLE_RESET_TOPIC);   
  sprintf(mqttPeopleCountThresholdTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_PEOPLE_COUNT_THRESHOLD_TOPIC);
  sprintf(mqttPeopleCountTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_PEOPLE_COUNT_TOPIC);
  sprintf(mqttSensorRebootTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_SENSOR_REBOOT_TOPIC);
  sprintf(mqttSensorResetTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_SENSOR_RESET_TOPIC);
  sprintf(mqttMeasurementBudgetTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_MEASUREMENT_BUDGET_TOPIC);
  sprintf(mqttMeasurementPeriodTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_MEASUREMENT_PERIOD_TOPIC);
  sprintf(mqttRoiConfigTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_ROI_CONFIG_TOPIC); 
  sprintf(mqttDistance1MeasurementTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_DISTANCE1_MEASUREMENT_TOPIC);
  sprintf(mqttDistance2MeasurementTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_DISTANCE2_MEASUREMENT_TOPIC);
  sprintf(mqttDistanceModeTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_DISTANCE_MODE_TOPIC);
  sprintf(mqttRangingPeriodTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_RANGING_PERIOD_TOPIC);
  sprintf(mqttFlashUpdateTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_FLASH_UPDATE_TOPIC);
  sprintf(mqttGetSensorConfigTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_GET_SENSOR_CONFIG_TOPIC);
  sprintf(mqttRestoreSensorConfigTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_RESTORE_SENSOR_CONFIG_TOPIC);
  sprintf(mqttWifiManagerEnableTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_WIFI_MANAGER_ENABLE_TOPIC);
  sprintf(mqttDummyTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_DUMMY_TOPIC);


  if (DEBUG) Serial.print("Wait for MQTT broker...");



  // Subscribe to topics and reconnect to MQTT server
  mqttReconnect();


}

void loop() {
  // put your main code here, to run repeatedly:
  bool distance1Flag = false;
  bool distance2Flag = false;
  char temp[50];
  String temp_str;
  unsigned long currentMillis = 0;
  uint16_t RangingData;
  //float standardDev;


  client.loop();

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
    //randomMqttClientName();
    mqttReconnect();
  }
  //------
  //------

  // inject the new ranged distance in the people counting algorithm
  //------
  RangingData = vl531Init(zone);
  
  client.loop();

  peopleCounterVarPrev = peopleCounterVar;
  peopleCounterVar = ProcessPeopleCountingData(RangingData, zone);

  if (zone == 0)
    mqttDistance1 = RangingData;

  if (zone == 1)
    mqttDistance2 = RangingData;

  zone++;
  zone = zone%2;

  client.loop();
  //------
  //------

  currentMillis = millis();
  // Check and publish the distance measurement 
  //------
  if ((currentMillis - measPreviousMillisRanging) >=  RANGING_PERIOD_MS) {
 

   // if (RangingData != 0) { // Check if we got meaningful distance data
    distance1Flag = true;
      
    temp_str = String(mqttDistance1); //converting ftemp (the float variable above) to a string

    // Add timestamp to distance measurement1, zone0
    temp_str.concat(',');
    //temp_str.concat(String(millis()));
      
    temp_str.toCharArray(temp, temp_str.length()); //packaging up the data to publish to mqtt whoa...

    client.publish(mqttDistance1MeasurementTopic, temp);  



    temp_str = String(mqttDistance2); //converting ftemp (the float variable above) to a string

    // Add timestamp to distance measurement2, zone1 
    temp_str.concat(',');
    //temp_str.concat(String(millis()));
      
    temp_str.toCharArray(temp, temp_str.length()); //packaging up the data to publish to mqtt whoa...

    client.publish(mqttDistance2MeasurementTopic, temp);  


    measPreviousMillisRanging =  millis();
  }

  currentMillis = millis();
  if ((currentMillis - measPreviousMillisDataSerialReport) >=  200) {

    measPreviousMillisDataSerialReport = millis();
  }
  //------
  //------

  currentMillis = millis();
  // Check and publish the people counter value every PEOPLE_COUNTER_PERIOD_MS ms
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

  // Check and publish the people counter uppon a change
  //------
  if (peopleCounterVar != peopleCounterVarPrev) {
      
    temp_str = String(peopleCounterVar); //converting ftemp (the float variable above) to a string

    // Add timestamp to distance measurement
    temp_str.concat(',');
    temp_str.concat(String(millis()));     

    temp_str.toCharArray(temp, temp_str.length() + 1); //packaging up the data to publish to mqtt whoa..

    client.publish(mqttPeopleCountTopic, temp);

    Serial.print("MQTT report, people counter: ");
    Serial.println(peopleCounterVar);    
  }
  //------
  //------

  // Keep MQTT connection active
  client.loop();


}