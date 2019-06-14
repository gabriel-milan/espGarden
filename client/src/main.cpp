/*
 *  Includes
 */
#include <Arduino.h>            // Using Arduino framework on ESP32
#include <WiFi.h>               // WiFi control for ESP32
#include <ThingsBoard.h>        // ThingsBoard SDK
#include "macros.h"             // Custom macros
#include <DHTesp.h>             // DHT for ESP32 library
#include <OneWire.h>            // ---
#include <DallasTemperature.h>  // Libraries for DS18B20 sensors

/*
 *  Globals
 */
float ambientTemperature;
float ambientHumidity;
float vaseTemp [2];
float vaseHum [4];
// DS18B20 adresses. Set to your own 64-bit addresses
DeviceAddress sensor1 = { 0x28, 0xFF, 0x77, 0x62, 0x40, 0x17, 0x4, 0x31 };
DeviceAddress sensor2 = { 0x28, 0xFF, 0xB4, 0x6, 0x33, 0x17, 0x3, 0x4B };
// HL-69 analog pins
int vaseHumPins [] = {SOIL_MOISTURE_PIN_1, SOIL_MOISTURE_PIN_2, SOIL_MOISTURE_PIN_3, SOIL_MOISTURE_PIN_4};

/*
 *  CPU tasks
 */
TaskHandle_t TaskThingsboardLoop;
TaskHandle_t TaskSendData;
TaskHandle_t TaskUpdateSensors;

/*
 *  Initializing Thingsboard stuff
 */
WiFiClient wifiClient;
ThingsBoard tb(wifiClient);

/*
 *  Initializing DHT22 sensor
 */
DHTesp dht;

/*
 *  Initializing OneWire and DallasTemperature
 */
OneWire oneWire (ONE_WIRE_BUS);
DallasTemperature sensors (&oneWire);

/*
 *  Method for WiFi connecting
 */
void ConnectToWifi (void) {

  if (DEBUG_MODE) {
    Serial.print("Connecting to ");
    Serial.print(WIFI_AP_NAME);
    Serial.print(" .");
  }

  WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (DEBUG_MODE)
      Serial.print(".");
  }
  
  if (DEBUG_MODE) {
    Serial.println();
    Serial.println("Connected to AP");
  }
}

/*
 *  Method for WiFi reconnection
 */
void ReconnectLoop (void) {

  int status = WiFi.status();

  if (status != WL_CONNECTED) {

    if (DEBUG_MODE)
      Serial.print("Connection down! Trying to reconnect.");

    WiFi.begin(WIFI_AP_NAME, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      if (DEBUG_MODE)
        Serial.print(".");
    }
    
    if (DEBUG_MODE)
      Serial.println("Connected to WiFi!");
  }
}

/*
 *  Method for Thingsboard connecting
 */
void ConnectToThingsboard (void) {

  // rpc_subscribed = false;

  if (DEBUG_MODE) {
    Serial.print("Connecting to: ");
    Serial.print(THINGSBOARD_SERVER);
    Serial.print(" with token ");
    Serial.println(TOKEN);
  }

  if (!tb.connect(THINGSBOARD_SERVER, TOKEN)) {
    if (DEBUG_MODE)
      Serial.println("Failed to connect");
  }
}

/*
 *  Function that reads the analog input from HL-69 sensor
 */
float GetVaseHumidity (int readPin) {
  return (float) map(analogRead(readPin), 0, 1023, 100, 0);
}

/*
 *  Task function for Thingsboard loop
 */
void ThingsboardLoop (void *pvParameters) {

  // Loop
  for (;;) {

    // Reconnecting to WiFi, if needed
    if (WiFi.status() != WL_CONNECTED) {
      if (DEBUG_MODE)
        Serial.println("WiFi not connected!!!");
      ReconnectLoop();
    }

    // Connecting to ThingsBoard, if needed
    if (!tb.connected()) {
      if (DEBUG_MODE)
        Serial.println("ThingsBoard not connected!!!");
      ConnectToThingsboard();
    }

    // Process messages
    tb.loop();

    // Delaying task
    vTaskDelay(pdMS_TO_TICKS(THINGSBOARD_LOOP_DELAY));
  }
}

/*
 *  Task function for sending data
 */
void SendData (void *pvParameters) {

  // Loop
  for (;;) {

    // Checking conditions
    if ((WiFi.status() == WL_CONNECTED) && (tb.connected())) {

      if (DEBUG_MODE)
        Serial.println("Sending data...");

      // Sending data
      tb.sendTelemetryFloat("ambientTemperature", ambientTemperature);
      tb.sendTelemetryFloat("ambientHumidity", ambientHumidity);
      tb.sendTelemetryFloat("vaseOneTemp", vaseTemp[0]);
      tb.sendTelemetryFloat("vaseTwoTemp", vaseTemp[1]);
      tb.sendTelemetryFloat("vaseOneHum", vaseHum[0]);
      tb.sendTelemetryFloat("vaseTwoHum", vaseHum[1]);
      tb.sendTelemetryFloat("vaseThreeHum", vaseHum[2]);
      tb.sendTelemetryFloat("vaseFourHum", vaseHum[3]);
    }
    else if (DEBUG_MODE) {
      Serial.println("Could not send data!");
    }

    // Delaying task
    vTaskDelay(pdMS_TO_TICKS(SENDING_DELAY));
  }
}

/*
 *  Task function for updating sensors
 */
void UpdateSensors (void *pvParameters) {

  // Loop
  for (;;) {

    if (DEBUG_MODE)
      Serial.println("Updating sensors...");

    // Updating values from DHT22
    TempAndHumidity tempHum = dht.getTempAndHumidity();
    if (isnan(tempHum.humidity) || isnan(tempHum.temperature)) {
      if (DEBUG_MODE)
        Serial.println("Failed to read from DHT22!");
    }
    else {
      ambientTemperature = tempHum.temperature;
      ambientHumidity = tempHum.humidity;
    }

    // Updating values from vase temperature sensor (DS18B20)
    vaseTemp[0] = sensors.getTempC(sensor1);
    vaseTemp[1] = sensors.getTempC(sensor2);

    // Updating values from soil moisture sensor (HL-69)
    for (byte i = 0; i < COUNT_OF(vaseHum); i++) {
      vaseHum[i] = GetVaseHumidity(vaseHumPins[i]);
    }

    // Delaying task
    vTaskDelay(pdMS_TO_TICKS(UPDATE_SENSORS_DELAY));
  }
}
/*
 *  Setup
 */
void setup (void) {

  // Debug
  if (DEBUG_MODE)
    Serial.begin(SERIAL_DEBUG_BAUD);

  // DHT22 sensor
  if (DEBUG_MODE)
    Serial.println("Setting DHT22 up...");
  dht.setup(DHT_PIN, DHTesp::DHT22);

  // Connecting to WiFi
  ConnectToWifi();

  // Create task for Thingsboard loop
  xTaskCreatePinnedToCore(
                    ThingsboardLoop,            /* Task function. */
                    "TaskThingsboardLoop",      /* Name of task. */
                    TASKS_STACK_SIZE,           /* Stack size of task */
                    NO_PARAMETERS,              /* Parameters of the task */
                    BASE_PRIORITY,              /* Priority of the task (the higher, the more important) */
                    &TaskThingsboardLoop,       /* Task handle to keep track of created task */
                    CORE_ZERO);                 /* Pin task to core 0 */
  vTaskDelay(pdMS_TO_TICKS(CREATE_TASK_DELAY)); 
  if (DEBUG_MODE)
    Serial.println("-- Created task ThingsboardLoop!");

  // Create task for SendData
  xTaskCreatePinnedToCore(
                    SendData,                   /* Task function. */
                    "TaskSendData",             /* Name of task. */
                    TASKS_STACK_SIZE,           /* Stack size of task */
                    NO_PARAMETERS,              /* Parameters of the task */
                    BASE_PRIORITY,              /* Priority of the task (the higher, the more important) */
                    &TaskSendData,              /* Task handle to keep track of created task */
                    CORE_ZERO);                 /* Pin task to core 0 */
  vTaskDelay(pdMS_TO_TICKS(CREATE_TASK_DELAY)); 
  if (DEBUG_MODE)
    Serial.println("-- Created task SendData!");

  // Create task for UpdateSensors
  xTaskCreatePinnedToCore(
                    UpdateSensors,              /* Task function. */
                    "TaskUpdateSensors",        /* Name of task. */
                    TASKS_STACK_SIZE,           /* Stack size of task */
                    NO_PARAMETERS,              /* Parameters of the task */
                    BASE_PRIORITY,              /* Priority of the task (the higher, the more important) */
                    &TaskUpdateSensors,         /* Task handle to keep track of created task */
                    CORE_ZERO);                 /* Pin task to core 0 */
  vTaskDelay(pdMS_TO_TICKS(CREATE_TASK_DELAY)); 
  if (DEBUG_MODE)
    Serial.println("-- Created task UpdateSensors!");

}

/*
 *  Loop
 */
void loop (void) {

  /*
   * Doing nothing here.
   * Loop method was only declared in order to stop complaints from the Arduino compiler.
   */
}