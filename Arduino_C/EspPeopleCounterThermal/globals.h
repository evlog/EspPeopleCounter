uint8_t EEPPROM_STR_LEN = 0;
const bool DEBUG = true; // Boolean variable used to enable debugging messages
int eeprom_addr = 0;

uint32_t OCCUPANCY_COUNTER_PERIOD_MS = 120000;

const char* REMOTE_PING_HOST = "www.google.com";

unsigned long measPreviousMillisRanging = 0;
unsigned long measPreviousMillisOccupancy = 0;
unsigned long measPreviousMillisDataSerialReport = 0;
unsigned long measPreviousMillisDeviationAlert = 0;
unsigned long measPreviousMillisReboot = 0;

uint16_t peopleCounter = 0;
uint16_t peopleCounterVar = 0;
uint16_t peopleCounterVarPrev = 0;
int zone = 0;

uint16_t mqttDistance1, mqttDistance2;

int16_t itemp_global = 0;

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


const char* MQTT_PEOPLE_COUNT_TOPIC = "people/count";  //**
char mqttPeopleCountTopic[150];

const char* MQTT_SENSOR_REBOOT_TOPIC = "reboot";  //**
char mqttSensorRebootTopic[150];

const char* MQTT_SENSOR_RESET_TOPIC = "reset";  //**
char mqttSensorResetTopic[150];

const char* MQTT_SAMPLE_TIME_TOPIC = "sampleTime";  //**
char mqttSampleTimeTopic[150];

const char* MQTT_FLASH_UPDATE_TOPIC = "flashUpdate"; //**
char mqttFlashUpdateTopic[150];

const char* MQTT_GET_SENSOR_CONFIG_TOPIC = "getSensorConfig";
char mqttGetSensorConfigTopic[150];

const char* MQTT_RESTORE_SENSOR_CONFIG_TOPIC = "restoreSensorConfig";
char mqttRestoreSensorConfigTopic[150];

const char* MQTT_WIFI_MANAGER_ENABLE_TOPIC = "wifiManagerEnable";
char mqttWifiManagerEnableTopic[150];

const char* MQTT_SENSOR_WIFI_TOPIC = "sensorWifi";
char mqttSensorWifiTopic[150];

const char* MQTT_COMPARING_NUM_INC_TOPIC = "comparingNumInc";
char mqttComparingNumIncTopic[150];

const char* MQTT_COMPARING_NUM_DEC_TOPIC = "comparingNumDec";
char mqttComparingNumDecTopic[150];

const char* MQTT_THRESH_HOLD_INC_TOPIC = "threshHoldInc";
char mqttthreshHoldIncTopic[150];

const char* MQTT_THRESH_HOLD_DEC_TOPIC = "threshHoldDec";
char mqttthreshHoldDecTopic[150];


void mqttCallback(char*, byte*, unsigned int); // This function is called when an MQTT message is received

WiFiClient wifiClient;
PubSubClient client(MQTT_ADDRESS, 1883, mqttCallback, wifiClient);

//const char* WIFI_SSID    = "Manyeleti";//"ubx";
//const char* WIFI_PASSWORD = "16148279";//"GqpZvmK8@r5yL#AP";

// Mark config
const char* WIFI_SSID    = "Phil UB";
const char* WIFI_PASSWORD = "";

int WIFI_MANAGER_ENABLE = 0;
//------
//------

// vl53l1 configruation and variables
// -----


// Define in ms how often to send ranging data for zone 1,2 over MQTT 
uint32_t  SAMPLE_TIME_MS = 10; // default is 10sec.


String MQTT_WIFI_SSID = "testSsid";
String MQTT_WIFI_PASSWORD = "12345678";

// Define distance metering mode of the sensor (short or long, default is long)
String  VL53L1_DISTANCE_MODE = "long";
//#######
char peopleCounterArray[50];


//Optional interrupt and shutdown pins
static int  SHUTDOWN_PIN1 = 2;    
static int  INTERRUPT_PIN1 = 3;
static int  SHUTDOWN_PIN2 = 4;    
static int  INTERRUPT_PIN2 = 5;


static int NOBODY = 0;
static int SOMEONE = 1;
static int UPPER_LEFT = 0;
static int LOWER_RIGHT = 1;
static int UPPER_RIGHT = 2;
static int LOWER_LEFT = 3;


uint32_t DIST_THRESHOLD_MAX[] = {1850, 1850};   // treshold of the two zones

static int PathTrack[] = {0,0,0,0};
static int PathTrackFillingSize = 1; // init this to 1 as we start from state where nobody is any of the zones
static int LeftPreviousStatus = NOBODY;
static int RightPreviousStatus = NOBODY;
static int PeopleCount = 0;

// Define ROI config1
static int center[4] = {93,229,165,29}; /* center of the two zones */  
static int Zone = 0;
static int PplCounter = 0;

static int ROI_height = 8;
static int ROI_width = 8;

// Standard deviation parameters
uint32_t SD_NUM_OF_SAMPLES = 10;
uint32_t SD_DEVIATION_THRESHOLD = 5;
bool deviationLowFlagPrev = false;
bool deviationHighFlagPrev = false;
bool deviationLowFlag = false;
bool deviationHighFlag = false;
// -----
// -----

// Thermal sensor parameters
// -----
/* defines */
#define D6T_ADDR 0x0A  // for I2C 7bit address
#define D6T_CMD 0x4C  // for D6T-44L-06/06H, D6T-8L-09/09H, for D6T-1A-01/02

#define N_ROW 1
#define N_PIXEL 1
#define N_READ ((N_PIXEL + 1) * 2 + 1)

#define SAMPLE_TIME_0009MS  9
#define SAMPLE_TIME_0010MS  10
#define SAMPLE_TIME_0012MS  12
#define SAMPLE_TIME_0015MS  15
#define SAMPLE_TIME_0020MS  20
#define SAMPLE_TIME_0040MS  40
#define SAMPLE_TIME_0060MS  60
#define SAMPLE_TIME_0100MS  100
#define SAMPLE_TIME_0200MS  200
#define SAMPLE_TIME_0400MS  400
#define SAMPLE_TIME_0800MS  800
#define SAMPLE_TIME_1600MS  1600
#define SAMPLE_TIME_3200MS  3200

#define PARA_0009MS_1 ((uint8_t)0x90)
#define PARA_0009MS_2 ((uint8_t)0xD3)
#define PARA_0009MS_3 ((uint8_t)0x29)
#define PARA_0010MS_1 ((uint8_t)0x90)
#define PARA_0010MS_2 ((uint8_t)0xD4)
#define PARA_0010MS_3 ((uint8_t)0x3C)
#define PARA_0012MS_1 ((uint8_t)0x90)
#define PARA_0012MS_2 ((uint8_t)0xD5)
#define PARA_0012MS_3 ((uint8_t)0x3B)
#define PARA_0015MS_1 ((uint8_t)0x90)
#define PARA_0015MS_2 ((uint8_t)0xD6)
#define PARA_0015MS_3 ((uint8_t)0x32)
#define PARA_0020MS_1 ((uint8_t)0x90)
#define PARA_0020MS_2 ((uint8_t)0xD7)
#define PARA_0020MS_3 ((uint8_t)0x35)
#define PARA_0040MS_1 ((uint8_t)0x90)
#define PARA_0040MS_2 ((uint8_t)0xD8)
#define PARA_0040MS_3 ((uint8_t)0x18)
#define PARA_0060MS_1 ((uint8_t)0x90)
#define PARA_0060MS_2 ((uint8_t)0xD9)
#define PARA_0060MS_3 ((uint8_t)0x1F)
#define PARA_0100MS_1 ((uint8_t)0x90)
#define PARA_0100MS_2 ((uint8_t)0xDA)
#define PARA_0100MS_3 ((uint8_t)0x16)
#define PARA_0200MS_1 ((uint8_t)0x90)
#define PARA_0200MS_2 ((uint8_t)0xDB)
#define PARA_0200MS_3 ((uint8_t)0x11)
#define PARA_0400MS_1 ((uint8_t)0x90)
#define PARA_0400MS_2 ((uint8_t)0xDC)
#define PARA_0400MS_3 ((uint8_t)0x04)
#define PARA_0800MS_1 ((uint8_t)0x90)
#define PARA_0800MS_2 ((uint8_t)0xDD)
#define PARA_0800MS_3 ((uint8_t)0x03)
#define PARA_1600MS_1 ((uint8_t)0x90)
#define PARA_1600MS_2 ((uint8_t)0xDE)
#define PARA_1600MS_3 ((uint8_t)0x0A)
#define PARA_3200MS_1 ((uint8_t)0x90)
#define PARA_3200MS_2 ((uint8_t)0xDF)
#define PARA_3200MS_3 ((uint8_t)0x0D)

/***** Setting Parameter 1 *****/
uint8_t comparingNumInc = 16;
//#define comparingNumInc 16 // x samplingTime ms   (range: 1 to 39)  (example) 16 x 100 ms -> 1.6 sec
uint8_t comparingNumDec = 16;
//#define comparingNumDec 16  // x samplingTime ms  (range: 1 to 39)  (example) 16 x 100 ms -> 1.6 sec
uint8_t threshHoldInc = 10;
//#define threshHoldInc 10 //  /10 degC   (example) 10 -> 1.0 degC (temperature change > 1.0 degC -> Enable)  
uint8_t threshHoldDec = 10;
//#define threshHoldDec 10 //  /10 degC   (example) 10 -> 1.0 degC (temperature change > 1.0 degC -> Disable)
//bool  enablePix[8] = {true, true, true, true, true, true, true, true};
/****************************/

/***** Setting Parameter 2 *****/
#define samplingTime SAMPLE_TIME_0100MS //ms (Can select only, 9ms, 10ms, 12ms, 15ms, 20ms, 40ms, 60ms, 100ms, 200ms, 400ms, 800ms, 1600ms, 3200ms)
/****************************/

uint8_t rbuf[N_READ];
int16_t pix_data = 0;
int16_t seqData[40] = {0};
bool  occuPix = 0;
bool  occuPixFlag = false;
uint8_t  resultOccupancy = 0;
uint8_t  resultOccupancy_prev = 0;
uint16_t  totalCount = 0;
// -----
// -----