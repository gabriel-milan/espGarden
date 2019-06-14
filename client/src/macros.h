// Debug
#define DEBUG_MODE				true

// Helper macro to calculate array size
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))

// WiFi macros
#define WIFI_AP_NAME            "YOUR_WIFI_NAME"
#define WIFI_PASSWORD           "YOUR_WIFI_PASSWORD"

// ThingsBoard macros
#define TOKEN                   "YOUR_TOKEN"
#define THINGSBOARD_SERVER      "YOUR_SERVER_IP"

// Tasks delays
#define CREATE_TASK_DELAY       500
#define THINGSBOARD_LOOP_DELAY	20
#define SENDING_DELAY		    1000
#define UPDATE_SENSORS_DELAY    500

// Multitasking macros
#define TASKS_STACK_SIZE        10000
#define BASE_PRIORITY           1
#define CORE_ZERO               0
#define CORE_ONE                1
#define NO_PARAMETERS           NULL

// Serial
#define SERIAL_DEBUG_BAUD       115200

// Pinout
#define DHT_PIN                 33
#define ONE_WIRE_BUS            15
#define SOIL_MOISTURE_PIN_1     36
#define SOIL_MOISTURE_PIN_2     39
#define SOIL_MOISTURE_PIN_3     34
#define SOIL_MOISTURE_PIN_4     35