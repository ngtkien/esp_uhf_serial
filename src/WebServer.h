#pragma once
#include "esp_uhf.h"
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "ArduinoJson.h"


extern AsyncWebSocket  ws;
// Set LED GPIO
extern const int ledPin;

void WebSetup();


void initWebSocket();


//handle
void mode_toggle();
String get_power();
void set_gain_handle(DynamicJsonDocument json);
void notify_led_msg();
void notify_gain_msg();
void notifyClients(String msg);