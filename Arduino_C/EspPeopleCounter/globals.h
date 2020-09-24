
const bool DEBUG = true; // Boolean variable used to enable debugging messages
int eeprom_addr = 0;

unsigned long measPreviousMillisRanging = 0;
unsigned long measPreviousMillisPeople = 0;
unsigned long measPreviousMillisDataSerialReport = 0;

uint16_t peopleCounter = 0;
uint16_t peopleCounterVar = 0;
uint16_t peopleCounterVarPrev = 0;
int zone = 0;

uint16_t mqttDistance1, mqttDistance2;

// Define the path of the binary file for flash update
String UPDATE_BINARY_FILE_PATH = "http://evlog.zapto.org/subs/figures/test.bin";

// Array to hold measurements for the standard deviation calculation
const int measArrSize = 500;
uint32_t measArr[measArrSize] = {};

// AP WiFi manager setup
// -----
WiFiManager wifiManager;
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

const char* MQTT_PEOPLE_COUNT_TOPIC = "people/count";
char mqttPeopleCountTopic[150];

const char* MQTT_SENSOR_REBOOT_TOPIC = "reboot";
char mqttSensorRebootTopic[150];

const char* MQTT_MEASUREMENT_BUDGET_TOPIC = "measurementBudget";
char mqttMeasurementBudgetTopic[150];

const char* MQTT_MEASUREMENT_PERIOD_TOPIC = "measurementPeriod";
char mqttMeasurementPeriodTopic[150];

const char* MQTT_ROI_CONFIG_TOPIC = "roiConfig";
char mqttRoiConfigTopic[150];

const char* MQTT_DISTANCE1_MEASUREMENT_TOPIC = "distance1Measurement";
char mqttDistance1MeasurementTopic[150];

const char* MQTT_DISTANCE2_MEASUREMENT_TOPIC = "distance2Measurement";
char mqttDistance2MeasurementTopic[150];

const char* MQTT_DISTANCE_MODE_TOPIC = "distanceMode";
char mqttDistanceModeTopic[150];

const char* MQTT_RANGING_PERIOD_TOPIC = "rangingPeriod";
char mqttRangingPeriodTopic[150];

const char* MQTT_FLASH_UPDATE_TOPIC = "flashUpdate";
char mqttFlashUpdateTopic[150];

const char* MQTT_GET_SENSOR_CONFIG_TOPIC = "getSensorConfig";
char mqttGetSensorConfigTopic[150];

const char* MQTT_RESTORE_SENSOR_CONFIG_TOPIC = "restoreSensorConfig";
char mqttRestoreSensorConfigTopic[150];

const char* MQTT_DUMMY_TOPIC = "dummy";
char mqttDummyTopic[150];


void mqttCallback(char*, byte*, unsigned int); // This function is called when an MQTT message is received

WiFiClient wifiClient;
PubSubClient client(MQTT_ADDRESS, 1883, mqttCallback, wifiClient);
//------
//------

// vl53l1 configruation and variables
// -----

// Timing budget set through VL53L1_SetMeasurementTimingBudgetMicroSeconds().
uint32_t  MEASUREMENT_BUDGET_MS = 50;

// Define in ms how often to send ranging data for zone 1,2 over MQTT 
uint32_t  RANGING_PERIOD_MS = 10000; // default is 10sec.

// Define in ms how often to send people counter data over MQTT 
uint32_t  PEOPLE_COUNTER_PERIOD_MS = 120000; //default is 2min. 

// Define in mm the people counter distance measurement threshold
uint32_t  PEOPLE_COUNT_THRESHOLD_MM = 0; 

uint32_t INTER_MEASUREMENT_PERIOD_MS = 55;

// Define distance metering mode of the sensor (short or long, default is long)
String  VL53L1_DISTANCE_MODE = "long";
//#######
char peopleCounterArray[50];


//Optional interrupt and shutdown pins
#define  SHUTDOWN_PIN 2    
#define  INTERRUPT_PIN 3


static int NOBODY = 0;
static int SOMEONE = 1;
static int LEFT = 0;
static int RIGHT = 1;

uint32_t DIST_THRESHOLD_MAX[] = {1850, 1650};   // treshold of the two zones

static int PathTrack[] = {0,0,0,0};
static int PathTrackFillingSize = 1; // init this to 1 as we start from state where nobody is any of the zones
static int LeftPreviousStatus = NOBODY;
static int RightPreviousStatus = NOBODY;
static int PeopleCount = 0;

// Define ROI config1
static int center[2] = {239,175}; /* center of the two zones */  
static int Zone = 0;
static int PplCounter = 0;

static int ROI_height = 5;
static int ROI_width = 5;
// -----
// -----
