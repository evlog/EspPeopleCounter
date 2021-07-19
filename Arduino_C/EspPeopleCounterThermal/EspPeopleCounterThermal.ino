// Define libraries
// -----
extern "C" {
#include "esp_wifi.h"
}
#include <WiFiManager.h> // Library to handle WiFi AP configuration portal 
#include <PubSubClient.h> // Library for MQTT
#include <Wire.h>
#include <StringSplitter.h>
#include "ESP32HTTPUpdate.h"
#include <EEPROM.h>
#include "globals.h"
// ----- 
// -----



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
  Serial.print("Client name: ");
  Serial.println(MQTT_CLIENT);
}
// ----- 
// -----


// Function to read EEPROM and initialize config. parameter
// -----
void mqttForceInitConfig() {
  SAMPLE_TIME_MS = 10000;    
  intToEeprom(SAMPLE_TIME_MS, 43); 
  WIFI_MANAGER_ENABLE = 1; 
  intToEeprom(WIFI_MANAGER_ENABLE, 79);
  MQTT_WIFI_SSID = "testSsid";
  strToEeprom(MQTT_WIFI_SSID, 155, MQTT_WIFI_SSID.length());
  MQTT_WIFI_PASSWORD = "12345678";
  strToEeprom(MQTT_WIFI_PASSWORD, 177, MQTT_WIFI_PASSWORD.length());
  comparingNumInc = 16;
  intToEeprom(comparingNumInc, 155);
  comparingNumDec = 16;
  intToEeprom(comparingNumDec, 161);
  threshHoldInc = 10;
  intToEeprom(threshHoldInc, 167);
  threshHoldDec = 10;
  intToEeprom(threshHoldDec, 173);
}
// -----
// -----

// Function to read EEPROM and initialize config. parameter
// -----
void initEepromConfigWrite() {    
  intToEeprom(SAMPLE_TIME_MS, 43); 
  intToEeprom(WIFI_MANAGER_ENABLE, 79);
  strToEeprom(MQTT_WIFI_SSID, 155, MQTT_WIFI_SSID.length());
  strToEeprom(MQTT_WIFI_PASSWORD, 177, MQTT_WIFI_PASSWORD.length());
  intToEeprom(comparingNumInc, 155);
  intToEeprom(comparingNumDec, 161);
  intToEeprom(threshHoldInc, 167);
  intToEeprom(threshHoldDec, 173);
}
// -----
// -----


// Function to store 10-digit str value to EEPROM
// -----
void strToEeprom(String param, int addr, int len) {

  String len_s;

  len_s = String(len);

  if (len < 10) {
    EEPROM.write(addr, 0);
    EEPROM.commit();
    addr+=1;
  
    EEPROM.write(addr, len_s.substring(0,1).toInt());  
    EEPROM.commit();
    addr+=1;
  }
  else {
    EEPROM.write(addr, len_s.substring(0,1).toInt());
    EEPROM.commit();
    addr+=1;
  
    EEPROM.write(addr, len_s.substring(1,2).toInt());
    EEPROM.commit();
    addr+=1;
  }
     
  for (int i = 0; i < len; i++) {
    EEPROM.write(addr, param.charAt(i));
    EEPROM.commit();
    addr+=1;
  }
}
// -----
// -----

String EepromToStr(int addr) {
  char data[20];
  byte b;
  EEPPROM_STR_LEN = 0;

  EEPPROM_STR_LEN +=  EEPROM.read(addr) * 10;
  EEPPROM_STR_LEN +=  EEPROM.read(addr+1) * 1;   

  Serial.print("EEPROM len:");
  Serial.println(EEPPROM_STR_LEN);

  if (EEPPROM_STR_LEN <= 20) {
    for(int i = 0; i < (EEPPROM_STR_LEN); i++) {
      b = EEPROM.read(addr+i+2);
      Serial.println((char)b);
      data[i] = (char) b;
    }
  }
  Serial.println(String(data));
  return String(data);
}

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
  SAMPLE_TIME_MS = EepromToInt(43);
  Serial.print("SAMPLE_TIME_MS:");
  Serial.println(SAMPLE_TIME_MS); 
  WIFI_MANAGER_ENABLE = EepromToInt(79); 
  Serial.print("WIFI_MANAGER_ENABLE:");
  Serial.println(WIFI_MANAGER_ENABLE);
  MQTT_WIFI_SSID = EepromToStr(155); 
  MQTT_WIFI_SSID = MQTT_WIFI_SSID.substring(0, EEPPROM_STR_LEN);
  Serial.print("MQTT_WIFI_SSID:"); 
  Serial.println(MQTT_WIFI_SSID);
  MQTT_WIFI_PASSWORD = EepromToStr(177); 
  MQTT_WIFI_PASSWORD = MQTT_WIFI_PASSWORD.substring(0, EEPPROM_STR_LEN);
  Serial.print("MQTT_WIFI_PASSWORD:");
  Serial.println(MQTT_WIFI_PASSWORD);
  comparingNumInc = EepromToInt(155); 
  Serial.print("comparingNumInc:");
  Serial.println(comparingNumInc);
  comparingNumDec = EepromToInt(161);     
  Serial.print("comparingNumDec:");
  Serial.println(comparingNumDec);
  threshHoldInc = EepromToInt(167);
  Serial.print("threshHoldInc:");
  Serial.println(threshHoldInc);
  threshHoldDec = EepromToInt(173);
  Serial.print("threshHoldDec:");
  Serial.println(threshHoldDec);
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


// This function is called when an MQTT message is received
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1];
  memcpy(p, payload, length);
  p[length] = NULL;
  String message(p);
  String topic_str(topic);
  char temp[400];
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
  if (topic_str == mqttSensorRebootTopic) {
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
  else if (topic_str == mqttSampleTimeTopic) {
    if (isValidNumber(message)) {
      if ((message.toInt() != 9) & (message.toInt() != 10) & (message.toInt() != 12) & (message.toInt() != 15) & (message.toInt() != 20) & (message.toInt() != 40) & (message.toInt() != 60) & (message.toInt() != 100) & (message.toInt() != 200) & (message.toInt() != 400) & (message.toInt() != 800) & (message.toInt() != 1600) & (message.toInt() != 3200)) {
        Serial.print(mqttSampleTimeTopic);
        Serial.println("->ERROR");
        client.publish(mqttSampleTimeTopic, "ERROR");
      }
      else {
        SAMPLE_TIME_MS = message.toInt();
        intToEeprom(SAMPLE_TIME_MS, 43);
        Serial.print(mqttSampleTimeTopic);
        Serial.println("->OK");
        client.publish(mqttSampleTimeTopic, "OK");
      }   
      if (DEBUG) { 
        Serial.print("mqttSampleTimeTopic -> ");
        Serial.println(SAMPLE_TIME_MS);
      }
    }
  }  
  else if (topic_str == mqttComparingNumIncTopic) {
    if (isValidNumber(message)) {
      if ((message.toInt() < 1) & (message.toInt() > 39)) {
        Serial.print(mqttComparingNumIncTopic);
        Serial.println("->ERROR");
        client.publish(mqttComparingNumIncTopic, "ERROR");
      }
      else {
        comparingNumInc = message.toInt();
        intToEeprom(comparingNumInc, 155);
        Serial.print(mqttComparingNumIncTopic);
        Serial.println("->OK");
        client.publish(mqttComparingNumIncTopic, "OK");
      }   
      if (DEBUG) { 
        Serial.print("mqttSampleTimeTopic -> ");
        Serial.println(comparingNumInc);
      }
    }
  }  
  else if (topic_str == mqttComparingNumDecTopic) {
    if (isValidNumber(message)) {
      if ((message.toInt() < 1) & (message.toInt() > 39)) {
        Serial.print(mqttComparingNumDecTopic);
        Serial.println("->ERROR");
        client.publish(mqttComparingNumDecTopic, "ERROR");
      }
      else {
        comparingNumDec = message.toInt();
        intToEeprom(comparingNumDec, 161);
        Serial.print(mqttComparingNumDecTopic);
        Serial.println("->OK");
        client.publish(mqttComparingNumDecTopic, "OK");
      }   
      if (DEBUG) { 
        Serial.print("mqttComparingNumDecTopic -> ");
        Serial.println(comparingNumDec);
      }
    }
  }  
  else if (topic_str == mqttthreshHoldIncTopic) {
    if (isValidNumber(message)) {
        threshHoldInc = message.toInt();
        intToEeprom(threshHoldInc, 167);
        Serial.print(mqttthreshHoldIncTopic);
        Serial.println("->OK");
        client.publish(mqttthreshHoldIncTopic, "OK");
      if (DEBUG) { 
        Serial.print("mqttthreshHoldIncTopic -> ");
        Serial.println(threshHoldInc);
      }
    }
  } 
  else if (topic_str == mqttthreshHoldDecTopic) {
    if (isValidNumber(message)) {
        threshHoldDec = message.toInt();
        intToEeprom(threshHoldDec, 173);
        Serial.print(mqttthreshHoldDecTopic);
        Serial.println("->OK");
        client.publish(mqttthreshHoldDecTopic, "OK"); 
      if (DEBUG) { 
        Serial.print("mqttthreshHoldDecTopic -> ");
        Serial.println(threshHoldDec);
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



  else if (topic_str == mqttSensorWifiTopic) {
    if (message.length() > 2) {
      String wifiCredentials = message;
      String ssid, passwd;
      StringSplitter *splitterWifi = new StringSplitter(wifiCredentials, ',', 2);

      //Read threshold
      ssid = splitterWifi->getItemAtIndex(0);
      passwd = splitterWifi->getItemAtIndex(1);

      if ((ssid.length() > 20) | (passwd.length() > 20)) {
        Serial.println("->ERROR");
        client.publish(mqttSensorWifiTopic, "ERROR");
      }
      else {
        Serial.print(mqttSensorWifiTopic);
        Serial.println("->OK");
        MQTT_WIFI_SSID = ssid;
        Serial.print("SSID length:");
        Serial.println(ssid.length());
        strToEeprom(MQTT_WIFI_SSID, 155, MQTT_WIFI_SSID.length());
        MQTT_WIFI_PASSWORD = passwd;
        Serial.print("Password length:");
        Serial.println(passwd.length());
        strToEeprom(MQTT_WIFI_PASSWORD, 177, MQTT_WIFI_PASSWORD.length());
        client.publish(mqttSensorWifiTopic, "OK");
         
        Serial.println(MQTT_WIFI_SSID);
        Serial.println(MQTT_WIFI_PASSWORD);
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


          temp_str.concat("|\nSAMPLE_TIME_MS: ");
          temp_str.concat(SAMPLE_TIME_MS);
          //temp_str.toCharArray(temp, temp_str.length() + 1);
          //client.publish(mqttGetSensorConfigTopic, temp);
          //Serial.println(temp_str);


          temp_str.concat("|\nWIFI_MANAGER_ENABLE: ");
          temp_str.concat(WIFI_MANAGER_ENABLE);
          //temp_str.toCharArray(temp, temp_str.length() + 1);
          //client.publish(mqttGetSensorConfigTopic, temp);
          //Serial.println(temp_str);

          temp_str.concat("|\nMQTT_WIFI_SSID: ");
          temp_str.concat(MQTT_WIFI_SSID);
          //temp_str.toCharArray(temp, temp_str.length() + 1);
          //client.publish(mqttGetSensorConfigTopic, temp);
          //Serial.println(temp_str);

          temp_str.concat("|\nMQTT_WIFI_PASSWORD: ");
          temp_str.concat(MQTT_WIFI_PASSWORD);
          //temp_str.toCharArray(temp, temp_str.length() + 1);
          //client.publish(mqttGetSensorConfigTopic, temp);
          //Serial.println(temp_str);

          temp_str.concat("|\comparingNumInc: ");
          temp_str.concat(comparingNumInc);
          //temp_str.toCharArray(temp, temp_str.length() + 1);
          //client.publish(mqttGetSensorConfigTopic, temp);
          //Serial.println(temp_str);

          temp_str.concat("|\comparingNumDec: ");
          temp_str.concat(comparingNumDec);
          //temp_str.toCharArray(temp, temp_str.length() + 1);
          //client.publish(mqttGetSensorConfigTopic, temp);
          //Serial.println(temp_str);

          temp_str.concat("|\threshHoldInc: ");
          temp_str.concat(threshHoldInc);
          //temp_str.toCharArray(temp, temp_str.length() + 1);
          //client.publish(mqttGetSensorConfigTopic, temp);
          //Serial.println(temp_str);

          temp_str.concat("|\threshHoldDec: ");
          temp_str.concat(threshHoldDec);
          //temp_str.toCharArray(temp, temp_str.length() + 1);
          //client.publish(mqttGetSensorConfigTopic, temp);
          //Serial.println(temp_str);

          temp_str.toCharArray(temp, temp_str.length() + 1);
          client.publish(mqttGetSensorConfigTopic, temp);
          Serial.println("Publish sensorConfig");
          Serial.println(temp_str);
        }
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
      
    Serial.println(mqttPeopleCountTopic);
    client.subscribe(mqttPeopleCountTopic); 
    Serial.println(mqttSensorRebootTopic);    
    client.subscribe(mqttSensorRebootTopic);
    Serial.println(mqttSensorResetTopic);    
    client.subscribe(mqttSensorResetTopic);      
    Serial.println(mqttSampleTimeTopic); 
    client.subscribe(mqttSampleTimeTopic); 
    Serial.println(mqttFlashUpdateTopic); 
    client.subscribe(mqttFlashUpdateTopic); 
    Serial.println(mqttGetSensorConfigTopic); 
    client.subscribe(mqttGetSensorConfigTopic); 
    Serial.println(mqttRestoreSensorConfigTopic); 
    client.subscribe(mqttRestoreSensorConfigTopic); 
    Serial.println(mqttWifiManagerEnableTopic); 
    client.subscribe(mqttWifiManagerEnableTopic); 
    Serial.println(mqttSensorWifiTopic);
    client.subscribe(mqttSensorWifiTopic);
    Serial.println(mqttComparingNumIncTopic);
    client.subscribe(mqttComparingNumIncTopic);
    Serial.println(mqttComparingNumDecTopic);
    client.subscribe(mqttComparingNumDecTopic);
    Serial.println(mqttthreshHoldIncTopic);
    client.subscribe(mqttthreshHoldIncTopic);
    Serial.println(mqttthreshHoldDecTopic);
    client.subscribe(mqttthreshHoldDecTopic);
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
  Serial.print("Client name: ");
  Serial.println(MQTT_CLIENT);
  //------
  //------
  
  if (WiFi.status() == WL_CONNECTED) {
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
  else {
    ESP.restart();
  }
}

void publichOccupancy(void) {
  char temp_occ[50];
  String temp_str_occ;
  Serial.print(mqttPeopleCountTopic);
  temp_str_occ.concat(String(resultOccupancy));
  temp_str_occ.concat(",");
  temp_str_occ.concat(String(itemp_global));
  temp_str_occ.toCharArray(temp_occ, temp_str_occ.length() + 1);
  Serial.print("->");
  Serial.println(temp_str_occ);
  client.publish(mqttPeopleCountTopic, temp_occ);
}

// JUDGE_occupancy: judge occupancy
bool judge_seatOccupancy(void) { 
  int j = 0; 
  for (j = 0; j < 39; j++){
    seqData[39 - j] = seqData[38 - j];
  }
  seqData[0] = pix_data;            
  if (totalCount <= comparingNumInc){
    totalCount++;
  }
  if (totalCount > comparingNumInc){    
    if (occuPix == false){
      if ((int16_t)(seqData[0] - seqData[comparingNumInc]) >= (int16_t)threshHoldInc){
        occuPix = true;
      }
    }
    else{   //resultOccupancy == true
      if ((int16_t)(seqData[comparingNumDec] - seqData[0]) >= (int16_t)threshHoldDec){
        occuPix = false;
      }
    }
    if (resultOccupancy == 0) {                
        if(occuPix == true){
          resultOccupancy = 1;
        }
    }
    else{
      occuPixFlag = false;
      if (occuPix == true){
        occuPixFlag = true;
      }
      if (occuPixFlag == false){
        resultOccupancy = 0;
      }
    }
  }
  return true;
}

// CRC calculation
uint8_t calc_crc(uint8_t data) {
    int index;
    uint8_t temp;
    for (index = 0; index < 8; index++) {
        temp = data;
        data <<= 1;
        if (temp & 0x80) {data ^= 0x07;}
    }
    return data;
}

//convert a 16bit data from the byte stream
int16_t conv8us_s16_le(uint8_t* buf, int n) {
    int ret;
    ret = buf[n];
    ret += buf[n + 1] << 8;
    return (int16_t)ret;   // and convert negative.
}

// D6T PEC(Packet Error Check) calculation
bool D6T_checkPEC(uint8_t buf[], int n) {
    int i;
    uint8_t crc = calc_crc((D6T_ADDR << 1) | 1);  // I2C Read address (8bit)
    for (i = 0; i < n; i++) {
        crc = calc_crc(buf[i] ^ crc);
    }
    bool ret = crc != buf[n];
    if (ret) {
        Serial.print("PEC check failed:");
        Serial.print(crc, HEX);
        Serial.print("(cal) vs ");
        Serial.print(buf[n], HEX);
        Serial.println("(get)");
    }
    return ret;
}

uint8_t para[3] = {0};
// Configure sampling time
void configSamplignTime() {
  switch(SAMPLE_TIME_MS){
    case SAMPLE_TIME_0009MS:
      para[0] = PARA_0009MS_1;
      para[1] = PARA_0009MS_2;
      para[2] = PARA_0009MS_3;
      break;
    case SAMPLE_TIME_0010MS:
      para[0] = PARA_0010MS_1;
      para[1] = PARA_0010MS_2;
      para[2] = PARA_0010MS_3;
      break;
    case SAMPLE_TIME_0012MS:
      para[0] = PARA_0012MS_1;
      para[1] = PARA_0012MS_2;
      para[2] = PARA_0012MS_3;
      break;
    case SAMPLE_TIME_0015MS:
      para[0] = PARA_0015MS_1;
      para[1] = PARA_0015MS_2;
      para[2] = PARA_0015MS_3;
      break;
    case SAMPLE_TIME_0020MS:
      para[0] = PARA_0020MS_1;
      para[1] = PARA_0020MS_2;
      para[2] = PARA_0020MS_3;
      break;
    case SAMPLE_TIME_0040MS:
      para[0] = PARA_0040MS_1;
      para[1] = PARA_0040MS_2;
      para[2] = PARA_0040MS_3;
      break;
    case SAMPLE_TIME_0060MS:
      para[0] = PARA_0060MS_1;
      para[1] = PARA_0060MS_2;
      para[2] = PARA_0060MS_3;
      break;
    case SAMPLE_TIME_0100MS:
      para[0] = PARA_0100MS_1;
      para[1] = PARA_0100MS_2;
      para[2] = PARA_0100MS_3;
      break;
    case SAMPLE_TIME_0200MS:
      para[0] = PARA_0200MS_1;
      para[1] = PARA_0200MS_2;
      para[2] = PARA_0200MS_3;
      break;
    case SAMPLE_TIME_0400MS:
      para[0] = PARA_0400MS_1;
      para[1] = PARA_0400MS_2;
      para[2] = PARA_0400MS_3;
      break;
    case SAMPLE_TIME_0800MS:
      para[0] = PARA_0800MS_1;
      para[1] = PARA_0800MS_2;
      para[2] = PARA_0800MS_3;
      break;
    case SAMPLE_TIME_1600MS:
      para[0] = PARA_1600MS_1;
      para[1] = PARA_1600MS_2;
      para[2] = PARA_1600MS_3;
      break;
    case SAMPLE_TIME_3200MS:
      para[0] = PARA_3200MS_1;
      para[1] = PARA_3200MS_2;
      para[2] = PARA_3200MS_3;
      break;
    default:
      para[0] = PARA_0100MS_1;
      para[1] = PARA_0100MS_2;
      para[2] = PARA_0100MS_3;
      break;
  }  
}


// Configure I2C parameters
void configI2cParam() {
  Wire.begin();
    Wire.beginTransmission(D6T_ADDR);  // I2C client address
    Wire.write(0x02);                  // D6T register
    Wire.write(0x00);                  // D6T register
    Wire.write(0x01);                  // D6T register
    Wire.write(0xEE);                  // D6T register
    Wire.endTransmission();            // I2C repeated start for read
    Wire.beginTransmission(D6T_ADDR);  // I2C client address
    Wire.write(0x05);                  // D6T register
    Wire.write(para[0]);                  // D6T register
    Wire.write(para[1]);                  // D6T register
    Wire.write(para[2]);                  // D6T register
    Wire.endTransmission();            // I2C repeated start for read
    Wire.beginTransmission(D6T_ADDR);  // I2C client address
    Wire.write(0x03);                  // D6T register
    Wire.write(0x00);                  // D6T register
    Wire.write(0x03);                  // D6T register
    Wire.write(0x8B);                  // D6T register
    Wire.endTransmission();            // I2C repeated start for read
    Wire.beginTransmission(D6T_ADDR);  // I2C client address
    Wire.write(0x03);                  // D6T register
    Wire.write(0x00);                  // D6T register
    Wire.write(0x07);                  // D6T register
    Wire.write(0x97);                  // D6T register
    Wire.endTransmission();            // I2C repeated start for read
    Wire.beginTransmission(D6T_ADDR);  // I2C client address
    Wire.write(0x02);                  // D6T register
    Wire.write(0x00);                  // D6T register
    Wire.write(0x00);                  // D6T register
    Wire.write(0xE9);                  // D6T register
    Wire.endTransmission();            // I2C repeated start for read    
}

void setup() {
  int i;
  
  // put your setup code here, to run once:
  Serial.begin (115200);  
  delay(500);
  Serial.println("Serial comm established**");
  Serial.print("Sensor MAC address: ");
  WiFi.mode(WIFI_MODE_STA);
 
  MAC_ADDRESS = WiFi.macAddress();
  Serial.println(MAC_ADDRESS);

  // Initialize I2C communication
  Wire.begin(); // Define here I2C pins, e.g. Wire.begin(3,4);

  // Initialize EPPROM memory
  EEPROM.begin(512);

  // Initialize measurements array
  for (i = 0; i < measArrSize; i++) 
    measArr[i] = 0;


  
  //Detect if this is the first boot and initialize in EEPROM the sensor configuration parameters 
  if (EEPROM.read(0) != 17) {
    Serial.println("Virgin boot");
    EEPROM.write(eeprom_addr, 17);
    EEPROM.commit();

    initEepromConfigWrite();
  }
  else {
    restoreEppromConfig();
  }

  Serial.println("WIFI_MANAGER_ENABLE:");
  Serial.println(WIFI_MANAGER_ENABLE);

  int x = 1;

  WiFi.disconnect(true);
  // Try to connect on fixed WiFi SSID and if not start the wifiManager
  if (WIFI_MANAGER_ENABLE == 0) {
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);

    delay(2000);

    delay(1000);
    if (WIFI_PASSWORD == " ")
      WiFi.begin(WIFI_SSID);
    else
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
    int wifiCounter = 0;
    Serial.println("Wait for WiFi fixed SSID...");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      wifiCounter++;
      if (wifiCounter == 40) {
        Serial.print("Failed to connect to fixed SSID. Trying MQTT provided WiFi credentials."); 
        break;
      }
    }

    // if first SSID failes try with the MQTT provided credentials
    if (WiFi.status() != WL_CONNECTED) {
      wifiCounter = 0;
      if (MQTT_WIFI_PASSWORD.c_str() == " ")
        WiFi.begin(MQTT_WIFI_SSID.c_str());
      else
        WiFi.begin(MQTT_WIFI_SSID.c_str(), MQTT_WIFI_PASSWORD.c_str());
      Serial.print("Connecting to ");
      Serial.println(MQTT_WIFI_SSID);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        wifiCounter++;
        if (wifiCounter == 40) {
          Serial.print("Failed to connect to fixed (MQTT provided) SSID. Restarting."); 
          ESP.restart();
          //break; 
        }
      }
    }
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
  // -----  


  // Connect to MQTT broker and subscribe to topics 
  //------
  client.setBufferSize(512); // Set buffer size to 512bytes to handle the getConfig command payload
  client.setCallback(mqttCallback);

  // Define MQTT topic names
  sprintf(mqttPeopleCountTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_PEOPLE_COUNT_TOPIC);
  sprintf(mqttSensorRebootTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_SENSOR_REBOOT_TOPIC);
  sprintf(mqttSensorResetTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_SENSOR_RESET_TOPIC); 
  sprintf(mqttSampleTimeTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_SAMPLE_TIME_TOPIC);
  sprintf(mqttFlashUpdateTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_FLASH_UPDATE_TOPIC);
  sprintf(mqttGetSensorConfigTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_GET_SENSOR_CONFIG_TOPIC);
  sprintf(mqttRestoreSensorConfigTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_RESTORE_SENSOR_CONFIG_TOPIC);
  sprintf(mqttWifiManagerEnableTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_WIFI_MANAGER_ENABLE_TOPIC);
  sprintf(mqttSensorWifiTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_SENSOR_WIFI_TOPIC);
  sprintf(mqttComparingNumIncTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_COMPARING_NUM_INC_TOPIC);
  sprintf(mqttComparingNumDecTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_COMPARING_NUM_DEC_TOPIC);
  sprintf(mqttthreshHoldIncTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_THRESH_HOLD_INC_TOPIC);
  sprintf(mqttthreshHoldDecTopic, "sensor/%s/%s", MAC_ADDRESS.c_str(), MQTT_THRESH_HOLD_DEC_TOPIC);


  if (DEBUG) Serial.print("Wait for MQTT broker...");


  // Subscribe to topics and reconnect to MQTT server 
  mqttReconnect(); 
  //------
  //------

  // Enable thermal sensor
  //------
  Serial.println("Enable Thermal sensor");
  configSamplignTime();
  configI2cParam();
  //------
  //------

}

void loop() {
  // put your main code here, to run repeatedly:
  bool distance1Flag = false;
  bool distance2Flag = false;
  char temp[50];
  String temp_str;
  unsigned long currentMillis = 0;
  uint16_t RangingData;
  float standardDev;


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

  // Read sensor
  //------
  int i, j;
  
  memset(rbuf, 0, N_READ);
  // Wire buffers are enough to read D6T-16L data (33bytes) with
  // MKR-WiFi1010 and Feather ESP32,
  // these have 256 and 128 buffers in their libraries.
  Wire.beginTransmission(D6T_ADDR);  // I2C client address
  Wire.write(D6T_CMD);               // D6T register
  Wire.endTransmission();            // I2C repeated start for read
  Wire.requestFrom(D6T_ADDR, N_READ);
  i = 0;
  while (Wire.available()) {
      rbuf[i++] = Wire.read();
  }

  if (D6T_checkPEC(rbuf, N_READ - 1)) {
      return;
  }

  // 1st data is PTAT measurement (: Proportional To Absolute Temperature)
  int16_t itemp = conv8us_s16_le(rbuf, 0);
  itemp_global = itemp;
  Serial.print("PTAT:");
  Serial.print(itemp / 10.0, 1);
  Serial.print(", Temperature:");

  // loop temperature pixels of each thrmopiles measurements
  for (i = 0, j = 2; i < N_PIXEL; i++, j += 2) {
      itemp = conv8us_s16_le(rbuf, j);
      pix_data = itemp;
      Serial.print(itemp / 10.0, 1);  // print PTAT & Temperature
      if ((i % N_ROW) == N_ROW - 1) {
          Serial.print(" [degC]");  // wrap text at ROW end.
      } else {
          Serial.print(", ");   // print delimiter
      }
  }
  judge_seatOccupancy(); //add
  Serial.print(", Occupancy:");
  Serial.println(resultOccupancy, 1); 
  if (resultOccupancy != resultOccupancy_prev)
    publichOccupancy();

  resultOccupancy_prev = resultOccupancy;
  delay(samplingTime);
  //------
  //------

  // Report occupancy status
  //------
  currentMillis = millis();
  if ((currentMillis - measPreviousMillisOccupancy) >=  OCCUPANCY_COUNTER_PERIOD_MS) {
    publichOccupancy();
    measPreviousMillisOccupancy = millis();
  }
  //------
  //------

  // Keep MQTT connection active
  client.loop();

}
