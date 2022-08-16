#pragma once

#include <Arduino.h>
#include "RF_Commands.h"
#include "HardwareSerial.h"
#include <Wire.h>
#include <extEEPROM.h>
#include <Mux.h>
#include "WebServer.h"

typedef enum {
  ROOM_1_ADDRESS = 0x0C,
  ROOM_2_ADDRESS = 0x60C,
  ROOM_3_ADDRESS = 0x120C,
  ROOM_4_ADDRESS = 0x180C,
  ROOM_5_ADDRESS = 0x1E0C
} ROOM_ADDR;
#define ROOM_NUMBER 5
#define ROOM_1_LENGTH_ADDR 0x0
#define ROOM_2_LENGTH_ADDR 0x600
#define ROOM_3_LENGTH_ADDR 0x1200
#define ROOM_4_LENGTH_ADDR 0x1800
#define ROOM_5_LENGTH_ADDR 0x1E00
extern RFC_Class rfc;
extern extEEPROM eep;         //device size, number of devices, page size


bool eep_save_tags(uint8_t room_number, uint8_t epc[]);
bool search_in_flash(uint8_t input[], ROOM_ADDR room);