const bool DEBUG = true; // Boolean variable used to enable debugging messages

unsigned long measPreviousMillis = 0;

int peopleCounter = 0;

// AP WiFi manager setup
// -----
// -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
const char thingName[] = "WiFiManager";

// -- Initial password to connect to the Thing, when it creates an own Access Point.
const char wifiInitialApPassword[] = "123456789";

DNSServer dnsServer;
WebServer server(80);

IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword);
// -----
// -----

// MQTT connection parameters
//*** Replace with your own MQTT username and password 
//------
String MAC_ADDRESS = WiFi.macAddress(); 

const char* MQTT_USERNAME = "pi";
const char* MQTT_PASSWORD = "rjaxtarmas1";

const char* MQTT_CLIENT = "testClien";  // *** Must be unique
const char* MQTT_ADDRESS = "evlog.zapto.org";

const char* MQTT_DEBUG_TOPIC = "debug";
char mqttDebugTopic[150];

const char* MQTT_PEOPLE_RESET_TOPIC = "people/reset";
char mqttPeopleResetTopic[150];

const char* MQTT_PEOPLE_COUNT_THRESHOLD_TOPIC = "threshold";
char mqttPeopleCountThresholdTopic[150];

const char* MQTT_SENSOR_REBOOT_TOPIC = "reboot";
char mqttSensorRebootTopic[150];

const char* MQTT_MEASUREMENT_BUDGET_TOPIC = "measurementBudget";
char mqttMeasurementBudgetTopic[150];

const char* MQTT_MEASUREMENT_PERIOD_TOPIC = "measurementPeriod";
char mqttMeasurementPeriodTopic[150];

const char* MQTT_ROI_CONFIG1_TOPIC = "roiConfig1";
char mqttRoiConfig1Topic[150];

const char* MQTT_ROI_CONFIG2_TOPIC = "roiConfig2";
char mqttRoiConfig2Topic[150];

const char* MQTT_DISTANCE1_MEASUREMENT_TOPIC = "distance1Measurement";
char mqttDistance1MeasurementTopic[150];

const char* MQTT_DISTANCE2_MEASUREMENT_TOPIC = "distance2Measurement";
char mqttDistance2MeasurementTopic[150];


void mqttCallback(char*, byte*, unsigned int); // This function is called when an MQTT message is received

WiFiClient wifiClient;
PubSubClient client(MQTT_ADDRESS, 1883, mqttCallback, wifiClient);
//------
//------

// vl53l1 configruation and variables
// -----
// By default, this example blocks while waiting for sensor data to be ready.
// Comment out this line to poll for data ready in a non-blocking way instead.
//#define USE_BLOCKING_LOOP

// Timing budget set through VL53L1_SetMeasurementTimingBudgetMicroSeconds().
uint32_t MEASUREMENT_BUDGET_MS = 50;

// Define in ms how often to send ranging data for zone 1,2 over MQTT 
uint32_t RANGING_PERIOD_MS = 10000; // default is 10sec.

// Define in ms how often to send people counter data over MQTT 
uint32_t PEOPLE_COUNTER_PERIOD_MS = 120000; //default is 2min. 

// Define in mm the people counter distance measurement threshold
uint32_t PEOPLE_COUNT_THRESHOLD_MM = 0; 

// People Counting defines
#define NOBODY 0
#define SOMEONE 1

// Interval between measurements, set through
// VL53L1_SetInterMeasurementPeriodMilliSeconds(). According to the API user
// manual (rev 2), "the minimum inter-measurement period must be longer than the
// timing budget + 4 ms." The STM32Cube example from ST uses 500 ms, but we
// reduce this to 55 ms to allow faster readings.
uint32_t INTER_MEASUREMENT_PERIOD_MS = 55;

// Define ROI config1
uint32_t config1TopLeftX = 0;
uint32_t config1TopLeftY = 0; 
uint32_t config1BottomRightX = 0; 
uint32_t config1BottomRightY = 0;

// Define ROI config2
uint32_t config2TopLeftX = 0;
uint32_t config2TopLeftY = 0; 
uint32_t config2BottomRightX = 0; 
uint32_t config2BottomRightY = 0;

VL53L1_Dev_t                   dev;
VL53L1_DEV                     Dev = &dev;

VL53L1_UserRoi_t roiConfig;

int status;
// -----
// -----
