const bool DEBUG = true; // Boolean variable used to enable debugging messages

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
const char* MQTT_USERNAME = "pi";
const char* MQTT_PASSWORD = "rjaxtarmas1";

const char* MQTT_CLIENT = "testClien";  // *** Must be unique
const char* MQTT_ADDRESS = "evlog.zapto.org";

const char* MQTT_DEBUG_TOPIC = "debug";
char mqttDebugTopic[150];

void mqttCallback(char*, byte*, unsigned int); // This function is called when an MQTT message is received

WiFiClient wifiClient;
PubSubClient client(MQTT_ADDRESS, 1883, mqttCallback, wifiClient);
//------
//------
