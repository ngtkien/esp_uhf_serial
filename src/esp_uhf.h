#pragma once

#include <Arduino.h>
#include "RF_Commands.h"
#include "HardwareSerial.h"
#include <Wire.h>
#include <extEEPROM.h>
#include <Mux.h>
#include "WebServer.h"

extern RFC_Class rfc;
extern extEEPROM eep;         //device size, number of devices, page size


bool eep_save_tags(uint8_t room_number, uint8_t epc[]);