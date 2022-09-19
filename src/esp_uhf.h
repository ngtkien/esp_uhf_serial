#pragma once

#include <Arduino.h>
#include "RF_Commands.h"
#include "HardwareSerial.h"
#include <Wire.h>
#include <extEEPROM.h>
#include <Mux.h>
#include "WebServer.h"

typedef enum {
  ROOM_1_ADDRESS = 0x008C,
  ROOM_2_ADDRESS = 0x00F0,
  ROOM_3_ADDRESS = 0x01B8,
  ROOM_4_ADDRESS = 0x02E4,
  ROOM_5_ADDRESS = 0x0474,
  ROOM_6_ADDRESS = 0x0668,
  ROOM_7_ADDRESS = 0x08C0,
  ROOM_8_ADDRESS = 0x0B7C,
  ROOM_9_ADDRESS = 0x0E9C,
  ROOM_10_ADDRESS = 0x1220,
  ROOM_11_ADDRESS = 0x1608,
  ROOM_12_ADDRESS = 0x1A54,
  ROOM_13_ADDRESS = 0x1F04,
  ROOM_14_ADDRESS = 0x2418,
  ROOM_15_ADDRESS = 0x2990,
  ROOM_16_ADDRESS = 0x2F6C
} ROOM_ADDR;
#define ROOM_NUMBER 16
#define ROOM_1_LENGTH_ADDR 0x0080
#define ROOM_2_LENGTH_ADDR 0x00E4
#define ROOM_3_LENGTH_ADDR 0x01AC
#define ROOM_4_LENGTH_ADDR 0x02D8
#define ROOM_5_LENGTH_ADDR 0x0468
#define ROOM_6_LENGTH_ADDR 0x065C
#define ROOM_7_LENGTH_ADDR 0x08B4
#define ROOM_8_LENGTH_ADDR 0x0B70
#define ROOM_9_LENGTH_ADDR 0x0E90
#define ROOM_10_LENGTH_ADDR 0x1214
#define ROOM_11_LENGTH_ADDR 0x15FC
#define ROOM_12_LENGTH_ADDR 0x1A48
#define ROOM_13_LENGTH_ADDR 0x1EF8
#define ROOM_14_LENGTH_ADDR 0x240C
#define ROOM_15_LENGTH_ADDR 0x2984
#define ROOM_16_LENGTH_ADDR 0x2F60

#define WORK_STATE_ADDR 0x00
#define TIMER_DELAY_ADDR 0x01 //0x01- 0x11
extern RFC_Class rfc;
extern extEEPROM eep;         //device size, number of devices, page size

bool eep_save_tags(uint8_t room_number, uint8_t epc[]);
bool search_in_flash(uint8_t input[], ROOM_ADDR room);
uint16_t get_size_room(ROOM_ADDR room);
bool eep_erase_room(uint8_t index);

void buzzer_done();