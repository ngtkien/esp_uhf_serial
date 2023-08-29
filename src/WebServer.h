#pragma once
#include "esp_uhf.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "ArduinoJson.h"

enum device_mote{
  NORMAL = 0,
  SAVE_TAGS
};

extern AsyncWebSocket  ws;
// Set LED GPIO
extern bool ledState;
extern bool ledMachineState;
extern const int ledPin;
// extern uint8_t delay1;
// extern uint8_t delay2;
// extern uint8_t delay3;
// extern uint8_t delay4;
// extern uint8_t delay5;
extern device_mote  _mode;
void WebSetup();


void initWebSocket();


//handle
void mode_toggle();
String get_power();
void set_gain_handle(DynamicJsonDocument json);
void notify_led_msg();
void notify_gain_msg();
void notifyClients(String msg);
void notifyInit();
void notifyTags(uint8_t epc[12]);

void cleanWifiStorage();
// Write file to SPIFFS
void writeFile(fs::FS &fs, const char * path, const char * message);
bool deleteFile(fs::FS &fs, const char* path);