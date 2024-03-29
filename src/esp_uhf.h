#pragma once

#include <Arduino.h>
#include "RF_Commands.h"
#include "HardwareSerial.h"
#include <Wire.h>
#include <extEEPROM.h>
#include <Mux.h>
#include "WebServer.h"

 #define PA_POWER 0x00
// typedef enum {
//   ROOM_1_ADDRESS = 0x008C,
//   ROOM_2_ADDRESS = 0x00F0,
//   ROOM_3_ADDRESS = 0x01B8,
//   ROOM_4_ADDRESS = 0x02E4,
//   ROOM_5_ADDRESS = 0x0474,
//   ROOM_6_ADDRESS = 0x0668,
//   ROOM_7_ADDRESS = 0x08C0,
//   ROOM_8_ADDRESS = 0x0B7C,
//   ROOM_9_ADDRESS = 0x0E9C,
//   ROOM_10_ADDRESS = 0x1220,
//   ROOM_11_ADDRESS = 0x1608,
//   ROOM_12_ADDRESS = 0x1A54,
//   ROOM_13_ADDRESS = 0x1F04,
//   ROOM_14_ADDRESS = 0x2418,
//   ROOM_15_ADDRESS = 0x2990,
//   ROOM_16_ADDRESS = 0x2F6C
// } ROOM_ADDR;
typedef enum {
  ROOM_1_ADDRESS = 0x8C,
  ROOM_2_ADDRESS = 0x7DF,
  ROOM_3_ADDRESS = 0xF32,
  ROOM_4_ADDRESS = 0x1685,
  ROOM_5_ADDRESS = 0x1DD8,
  ROOM_6_ADDRESS = 0x252B,
  ROOM_7_ADDRESS = 0x2C7E,
  ROOM_8_ADDRESS = 0x33D1,
  ROOM_9_ADDRESS = 0x3B24,
  ROOM_10_ADDRESS = 0x4277,
  ROOM_11_ADDRESS = 0x49CA,
  ROOM_12_ADDRESS = 0x511D,
  ROOM_13_ADDRESS = 0x5870,
  ROOM_14_ADDRESS = 0x5FC3,
  ROOM_15_ADDRESS = 0x6716,
  ROOM_16_ADDRESS = 0x6E69
} ROOM_ADDR;


#define ROOM_NUMBER 16
#define MAX_TAGS 100
// #define ROOM_1_LENGTH_ADDR 0x0080
// #define ROOM_2_LENGTH_ADDR 0x00E4
// #define ROOM_3_LENGTH_ADDR 0x01AC
// #define ROOM_4_LENGTH_ADDR 0x02D8
// #define ROOM_5_LENGTH_ADDR 0x0468
// #define ROOM_6_LENGTH_ADDR 0x065C
// #define ROOM_7_LENGTH_ADDR 0x08B4
// #define ROOM_8_LENGTH_ADDR 0x0B70
// #define ROOM_9_LENGTH_ADDR 0x0E90
// #define ROOM_10_LENGTH_ADDR 0x1214
// #define ROOM_11_LENGTH_ADDR 0x15FC
// #define ROOM_12_LENGTH_ADDR 0x1A48
// #define ROOM_13_LENGTH_ADDR 0x1EF8
// #define ROOM_14_LENGTH_ADDR 0x240C
// #define ROOM_15_LENGTH_ADDR 0x2984
// #define ROOM_16_LENGTH_ADDR 0x2F60
#define ROOM_1_LENGTH_ADDR 0x80
#define ROOM_2_LENGTH_ADDR 0x7D3
#define ROOM_3_LENGTH_ADDR 0xF26
#define ROOM_4_LENGTH_ADDR 0x1679
#define ROOM_5_LENGTH_ADDR 0x1DCC
#define ROOM_6_LENGTH_ADDR 0x251F
#define ROOM_7_LENGTH_ADDR 0x2C72
#define ROOM_8_LENGTH_ADDR 0x33C5
#define ROOM_9_LENGTH_ADDR 0x3B18
#define ROOM_10_LENGTH_ADDR 0x426B
#define ROOM_11_LENGTH_ADDR 0x49BE
#define ROOM_12_LENGTH_ADDR 0x5111
#define ROOM_13_LENGTH_ADDR 0x5864
#define ROOM_14_LENGTH_ADDR 0x5FB7
#define ROOM_15_LENGTH_ADDR 0x670A
#define ROOM_16_LENGTH_ADDR 0x6E5D


typedef struct {
    bool res;
    uint32_t addr;
    uint8_t index_in_room;
    uint8_t index_room;
    ROOM_ADDR addr_room;
} search_result_t;
//LED STATE
#define LED_STASTE_PIN 15
#define WORK_STATE_ADDR 0x00
#define TIMER_DELAY_ADDR 0x01 //0x01- 0x11
extern RFC_Class rfc;
extern extEEPROM eep;         //device size, number of devices, page size

bool eep_save_tags(uint8_t room_number, uint8_t epc[]);
search_result_t search_in_flash(uint8_t input[], ROOM_ADDR room);
uint16_t get_size_room(ROOM_ADDR room);
bool eep_erase_room(uint8_t index);
search_result_t eep_find_tags(uint8_t epc[]);
bool eep_delete_tags(uint8_t epc[]);
void buzzer_done();