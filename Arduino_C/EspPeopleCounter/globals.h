const char* FIRMWARE_VERSION = "firmwarev1";

// Timer ping conenctivity check interrupt
// -----
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

bool checkWifiFlag = false;
// -----
// -----

uint8_t EEPPROM_STR_LEN = 0;
const bool DEBUG = true; // Boolean variable used to enable debugging messages
int eeprom_addr = 0;

const char* REMOTE_PING_HOST = "www.google.com";

unsigned long measPreviousMillisRanging = 0;
unsigned long measPreviousMillisPeople = 0;
unsigned long measPreviousMillisDataSerialReport = 0;
unsigned long measPreviousMillisDeviationAlert = 0;
unsigned long measPreviousMillisReboot = 0;

uint16_t peopleCounter = 0;
uint16_t peopleCounterVar = 0;
uint16_t peopleCounterVarPrev = 0;
int zone = 0;

uint16_t mqttDistance1, mqttDistance2;

// Define the path of the binary file for flash update
String UPDATE_BINARY_FILE_PATH = "http://update.switchfi.co.za/firmware/counteresp32.bin"; //"http://evlog.zapto.org/subs/figures/test.bin";

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
String MAC_ADDRESS;

//const char* MQTT_USERNAME = "pi";
//const char* MQTT_PASSWORD = "rjaxtarmas1"; 

//const char* MQTT_CLIENT = "testClien";  // *** Must be unique
//const char* MQTT_ADDRESS = "esp-32.zapto.org";

// Mark config
const char* MQTT_USERNAME = "switchfi";
const char* MQTT_PASSWORD = "M@rkmj1989"; 

const char* MQTT_CLIENT = "testClien";  // *** Must be unique
const char* MQTT_ADDRESS = "mqtt.switchfi.co.za";

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

const char* MQTT_SENSOR_RESET_TOPIC = "reset";
char mqttSensorResetTopic[150];

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

const char* MQTT_DISTANCES_ARRAY_SIZE_TOPIC = "distancesArraySize";
char mqttDistancesArraySizeTopic[150];

const char* MQTT_MAX_DISTANCE_TOPIC = "maxDistance";
char mqttMaxDistanceTopic[150];

const char* MQTT_MIN_DISTANCE_TOPIC = "minDistance";
char mqttMinDistanceTopic[150];

const char* MQTT_FLASH_UPDATE_TOPIC = "flashUpdate";
char mqttFlashUpdateTopic[150];

const char* MQTT_GET_SENSOR_CONFIG_TOPIC = "getSensorConfig";
char mqttGetSensorConfigTopic[150];

const char* MQTT_RESTORE_SENSOR_CONFIG_TOPIC = "restoreSensorConfig";
char mqttRestoreSensorConfigTopic[150];

const char* MQTT_DEVIATION_DATA_TOPIC = "deviation/data";
char mqttDeviationDataTopic[150];

const char* MQTT_DEVIATION_VALUE_HIGH_TOPIC = "deviation/value/high";
char mqttDeviationValueHighTopic[150];

const char* MQTT_DEVIATION_VALUE_LOW_TOPIC = "deviation/value/low";
char mqttDeviationValueLowTopic[150];

const char* MQTT_WIFI_MANAGER_ENABLE_TOPIC = "wifiManagerEnable";
char mqttWifiManagerEnableTopic[150];

const char* MQTT_SENSOR_WIFI_TOPIC = "sensorWifi";
char mqttSensorWifiTopic[150];

const char* MQTT_SHUTDOWN_SENSOR1_TOPIC = "shutdownSensor1";
char mqttShutdownSensor1Topic[150];

const char* MQTT_SHUTDOWN_SENSOR2_TOPIC = "shutdownSensor2";
char mqttShutdownSensor2Topic[150];

const char* MQTT_DUMMY_TOPIC = "dummy";
char mqttDummyTopic[150];


void mqttCallback(char*, byte*, unsigned int); // This function is called when an MQTT message is received

WiFiClient wifiClient;
PubSubClient client(MQTT_ADDRESS, 1883, mqttCallback, wifiClient);

const char* WIFI_SSID    = "Phil UB";//"Manyeleti";//"ubx";
const char* WIFI_PASSWORD = "12312C64400151";//"16148279";//"GqpZvmK8@r5yL#AP";

// Mark config
//const char* WIFI_SSID    = "Intuitive";
//const char* WIFI_PASSWORD = "SensorS123";

int WIFI_MANAGER_ENABLE = 0;
//------
//------

// vl53l1 configruation and variables
// -----

// Timing budget set through VL53L1_SetMeasurementTimingBudgetMicroSeconds().
uint32_t  MEASUREMENT_BUDGET_MS = 33;

// Define in ms how often to send ranging data for zone 1,2 over MQTT 
uint32_t  RANGING_PERIOD_MS = 10000; // default is 10sec.

// Define in ms how often to send people counter data over MQTT 
uint32_t  PEOPLE_COUNTER_PERIOD_MS = 120000; //default is 2min. 

// Define in ms how often to send deviation alert report
uint32_t DEVIATION_COUNTER_PERIOD_MS = 120000; //default is 2min. 

// Define in mm the people counter distance measurement threshold
uint32_t  PEOPLE_COUNT_THRESHOLD_MM = 0; 

uint32_t INTER_MEASUREMENT_PERIOD_MS = 33;

uint32_t DISTANCES_ARRAY_SIZE = 4;

uint32_t MAX_DISTANCE = 33;

uint32_t MIN_DISTANCE = 0;

String MQTT_WIFI_SSID = "Intuitive";
String MQTT_WIFI_PASSWORD = "SensorS123";

// Define distance metering mode of the sensor (short or long, default is long)
String  VL53L1_DISTANCE_MODE = "long";
//#######
char peopleCounterArray[50];


//Optional interrupt and shutdown pins
static int  SHUTDOWN_PIN1 = 25;    
static int  INTERRUPT_PIN1 = 26;
static int  SHUTDOWN_PIN2 = 19;    
static int  INTERRUPT_PIN2 = 18;


static int NOBODY = 0;
static int SOMEONE = 1;
static int UPPER_LEFT = 0;
static int LOWER_RIGHT = 1;
static int UPPER_RIGHT = 2;
static int LOWER_LEFT = 3;


uint32_t DIST_THRESHOLD_MAX[] = {1950, 1950};   // treshold of the two zones

static int PathTrack[] = {0,0,0,0};
static int PathTrackFillingSize = 1; // init this to 1 as we start from state where nobody is any of the zones
static int LeftPreviousStatus = NOBODY;
static int RightPreviousStatus = NOBODY;
static int PeopleCount = 0;

// Define ROI config1
static int center_1[2] = {93, 229}; /* center of the two zones */  
static int Zone_1 = 0;
static int PplCounter_1 = 0;

static int ROI_height_1 = 8;
static int ROI_width_1 = 8;

// Define ROI config2
static int center_2[2] = {93, 229}; /* center of the two zones */  
static int Zone_2 = 0;
static int PplCounter = 0;

static int ROI_height_2 = 8;
static int ROI_width_2 = 8;

// Standard deviation parameters
uint32_t SD_NUM_OF_SAMPLES = 10;
uint32_t SD_DEVIATION_THRESHOLD = 5;
bool deviationLowFlagPrev = false;
bool deviationHighFlagPrev = false;
bool deviationLowFlag = false;
bool deviationHighFlag = false;
// -----
// -----
