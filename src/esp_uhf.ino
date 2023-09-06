
#include <Arduino.h>
#include "esp_uhf.h"
#include "EasyBuzzer.h"
#include "OneButton.h"
unsigned int frequency = 1000;
unsigned int duration = 300;

unsigned int wifi_manger_en = 0;

using namespace admux;


/*
 * Creates a Mux instance.
 *
 * 1st argument is the SIG (signal) pin (Arduino digital output pin 3).
 * 2nd argument is the S0-S3 (channel control) pins (Arduino pins 8, 9, 10, 11).
 */
//
#define STATE_SWITCH  4

//Multiplex
#define SIG 14
#define EN 26
#define S0 25
#define S1 33
#define S2 32
#define S3 27
// #define RFIDEN 14
OneButton rs_btn(STATE_SWITCH);
Mux mux(Pin(SIG, OUTPUT, PinType::Digital), Pinset(S0, S1,S2, S3));
HardwareSerial SerialRF(2);
RFC_Class rfc(&SerialRF);
const uint32_t totalKBytes = 32;         //for read and write test functions
extEEPROM eep(kbits_256, 1, 64);         //device size, number of devices, page size
extern uint8_t Gain;
bool active = false;
byte epc_list[4][12] = {
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0xA8, 0x4B},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0x6A, 0x19},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x18},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x1B}
};

byte temp_epcp[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0 , 0, 0, 0};
byte room1[10][12] = {
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0xA8, 0x40},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0xA8, 0x41},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0xA8, 0x42},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0xA8, 0x43},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0xA8, 0x44},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0xA8, 0x45},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0xA8, 0x46},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0xA8, 0x47},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0xA8, 0x48},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0xA8, 0x4B}, //Real
};
byte room2[10][12] = {
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0x6A, 0x10},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0x6A, 0x11},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0x6A, 0x12},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0x6A, 0x13},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0x6A, 0x14},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0x6A, 0x15},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0x6A, 0x16},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0x6A, 0x17},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0x6A, 0x20},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0x6A, 0x19}, //Real
};
byte room3[10][12] = {
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x21},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x22},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x23},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x24},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x25},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x26},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x27},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x28},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x29},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x18}, //Real
};
byte room4[10][12] = {
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x30},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x31},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x32},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x33},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x34},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x35},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x36},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x37},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x38},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x1B} //Real
};

ROOM_ADDR table[ROOM_NUMBER] = {
    ROOM_ADDR::ROOM_1_ADDRESS, 
    ROOM_ADDR::ROOM_2_ADDRESS, 
    ROOM_ADDR::ROOM_3_ADDRESS, 
    ROOM_ADDR::ROOM_4_ADDRESS,
    ROOM_ADDR::ROOM_5_ADDRESS,
    ROOM_ADDR::ROOM_6_ADDRESS ,
    ROOM_ADDR::ROOM_7_ADDRESS ,
    ROOM_ADDR::ROOM_8_ADDRESS ,
    ROOM_ADDR::ROOM_9_ADDRESS,
    ROOM_ADDR::ROOM_10_ADDRESS,
    ROOM_ADDR::ROOM_11_ADDRESS,
    ROOM_ADDR::ROOM_12_ADDRESS,
    ROOM_ADDR::ROOM_13_ADDRESS,
    ROOM_ADDR::ROOM_14_ADDRESS,
    ROOM_ADDR::ROOM_15_ADDRESS,
    ROOM_ADDR::ROOM_16_ADDRESS
};
byte test_source[12] = {0xE2,0x80,0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x18};
byte data[12] = {0x0};

extern uint8_t tm_delay[ROOM_NUMBER];
void onRelay(ROOM_ADDR room, uint32_t delay){
    for(int i = 0; i < ROOM_NUMBER; i++ ){
        if(room == table[i]){
            mux.write(LOW, i);
            delay(delay*1000);
            mux.write(HIGH, i);
        }
    }
}
void onLed(ROOM_ADDR room){
    for(int i = 0; i < ROOM_NUMBER; i++ ){
        if(room == table[i]){
            mux.write(LOW, i);
            delay(200);
            mux.write(HIGH, i);
        }
    }
    // switch (room)
    // {
    // case ROOM_ADDR::ROOM_1_ADDRESS:
    //     Serial.printf("Open Gate: %d, delay %d\n", 1,delay1);
        
    //     mux.write(LOW, 0);
    //     delay(delay1*1000);
    //     mux.write(HIGH, 0);
    //     /* code */
    //     break;
    // case ROOM_ADDR::ROOM_2_ADDRESS:
    //     Serial.printf("Open Gate: %d, delay %d\n", 2,delay2);
        
    //     mux.write(LOW, 1);
    //     delay(delay2*1000);
    //     mux.write(HIGH, 1);
    //     /* code */
    //     break;
    // case ROOM_ADDR::ROOM_3_ADDRESS:
    //     Serial.printf("Open Gate: %d, delay %d\n", 3,delay3);
    //     mux.write(LOW, 2);
    //     delay(delay3*1000);
    //     mux.write(HIGH, 2);
    //     /* code */
    //     break;
    // case ROOM_ADDR::ROOM_4_ADDRESS:
    //     Serial.printf("Open Gate: %d, delay %d\n", 4,delay4);
        
    //     mux.write(LOW, 3);
    //     delay(delay4*1000);
    //     mux.write(HIGH, 3);
    //     /* code */
    //     break;
    // case ROOM_ADDR::ROOM_5_ADDRESS:
    //     Serial.printf("Open Gate: %d, delay %d\n", 5,delay5);
    //     mux.write(LOW, 4);
    //     delay(delay5*1000);
    //     mux.write(HIGH, 4);
    //     /* code */
    //     break;
    // default:
    //     break;
    // }
}
void eeErase(uint8_t chunk, uint32_t startAddr, uint32_t endAddr)
{
  chunk &= 0xFC;                //force chunk to be a multiple of 4
  uint8_t data[chunk];
  Serial.println(F("Erasing..."));
  for (int i = 0; i < chunk; i++) data[i] = 0xFF;
  uint32_t msStart = millis();

  for (uint32_t a = startAddr; a <= endAddr; a += chunk) {
    if ( (a & 0xFFF) == 0 ) Serial.println(a);
    eep.write(a, data, chunk);
  }
  uint32_t msLapse = millis() - msStart;
  Serial.print(F("Erase lapse: "));
  Serial.print(msLapse);
  Serial.print(F(" ms"));
}
void test_write(){
    Serial.println("TEST WRITE");


    // // eep.write(ROOM_1_LENGTH_ADDR + 1 ,  0x0A); //=> 0x000A
    // // eep.write(ROOM_1_LENGTH_ADDR ,  0x00);
    uint32_t index;

    Serial.println("Room 5-1\n");
    index = ROOM_ADDR::ROOM_4_ADDRESS;
    for(uint8_t i = 0; i < 10; i++){
        Serial.printf("Write at: 0x%04x\n", index);
        // eep.write(index,room4[i],12);
        index = index + 12;
        //

    }

    Serial.println("Write in B");
    index = ROOM_ADDR::ROOM_4_ADDRESS;
    for(uint8_t i = 0; i < 0x0B; i++){
        Serial.printf("Write at: 0x%04x\n", index);
        if(i == 0x0B-1){
           Serial.printf("Write at B: 0x%04x\n", index);
        }
        // eep.write(index,room4[i],12);
        index = index + 12;

    }
    ROOM_ADDR room = table[4];
    uint16_t size = 0x0A;
    uint32_t index2 = room + size*12;
     Serial.printf("Write add 0x%04x ",index2);

     increase_size_room(room);
}
void test_eeprom(){
    
    Serial.println("TEST READ");

    uint8_t maxDataSize = 12;
    memset(data, 0, maxDataSize);
    eep.read(ROOM_1_ADDRESS,data,maxDataSize);
    for(int i = 0; i < maxDataSize ; i++){
        Serial.printf("%02X", data[i], HEX);
    }
    Serial.println();

    memset(data, 0, maxDataSize);
    eep.read(ROOM_2_ADDRESS + 12,data,maxDataSize);
    for(int i = 0; i < maxDataSize ; i++){
        Serial.printf("%02X", data[i], HEX);
    }
    Serial.println();

    memset(data, 0, maxDataSize);
    eep.read(ROOM_3_ADDRESS + 108,data,maxDataSize);
    for(int i = 0; i < maxDataSize ; i++){
        Serial.printf("%02X", data[i], HEX);
    }
    Serial.println();

    memset(data, 0, maxDataSize);
    eep.read(ROOM_4_ADDRESS + 36,data,maxDataSize);
    for(int i = 0; i < maxDataSize ; i++){
        Serial.printf("%02X", data[i], HEX);
    }
    Serial.println();
}
search_result_t eep_find_tags( uint8_t epc[]){
    search_result_t res;
    res.res = false;
    for(uint8_t i = 0; i < ROOM_NUMBER; i++){
        search_result_t res = search_in_flash(epc,table[i]);
        if(res.res) {
            Serial.printf("Finding it in Room address 0x%04X\n", table[i]);
            res.index_room = i + 1;
            res.addr_room = table[i];
            return res;
        }
    }
    return res;
}

bool eep_delete_tags( uint8_t epc[]){
    Serial.println("eep delete tags");
    search_result_t res;
    res =  eep_find_tags(epc);
    if(!res.res){
        Serial.println("Not find tags");
        return false;
    }

    uint8_t size = get_size_room(res.addr_room);
    Serial.printf("Size: %d \nfind tags:tags addr: 0x%04X, tags_ind: %d, 0x%04X\n",size ,res.addr, res.index_in_room, res.addr_room);

    for(int i = 0; i < 12;i++){
        
        eep.write(res.addr + i,0xFF);
    }
    uint8_t _data[12];
    uint8_t _size;
    Serial.printf("deletags\n");

    if(res.index_in_room < (size - 1)){
        //Read the last tags and store its in erase size
        uint16_t _addr = res.addr_room + (size - 1 )*12;
        Serial.printf("Re-sort data in flash, get data from: 0x%04X\n",_addr);
        eep.read(_addr,_data,12);
        for(uint8_t i = 0; i < 12; i++){
            Serial.printf("%02X",_data[i]);
        }
        Serial.println();
        //Write to erased 
        _size = eep.write(res.addr,_data,12);
        // if(_size != 12){
        //     return false;
        // }

        //Decrease size room
        decrease_size_room(res.addr_room);
        return true;
    }

    decrease_size_room(res.addr_room);
    return true;
}
bool eep_save_tags(uint8_t room_number, uint8_t epc[]){

    for(int i = 0; i < ROOM_NUMBER; i++){
        search_result_t res = search_in_flash(epc,table[i]);
        if(res.res) {
            Serial.printf("Find it in Room address 0x%04X\n", table[i]);
            return false;
        }
    }
    if(room_number > ROOM_NUMBER){
        Serial.println("Room not exists");
        return false;
    }
    //Get room_number -> room address and room length
    //Calculator index 
    ROOM_ADDR room = table[room_number-1];
    uint16_t size = get_size_room(room);
    uint32_t index = room;
    if(size > 99){
        return false;
    }
    if(size == 0){
        index = room;
    }
    else {
        index = room + (size-1)*12 + 12;
    }
    Serial.printf("Save in addr: 0x%04X room number: %d room addr: 0x%04X\n",index, room_number,room);
    //TODO: Write in the next last index
    uint8_t _data[12];
    eep.write(index,epc,12);
    memset(_data, 0, 12);
    eep.read(index,_data,12);
    if(compare(epc, _data)){
        increase_size_room(room);
        return true;
    }
    else {
            //Next offset
        Serial.println("Write failed");
        return false;
    }
    //TODO: Increse length
    
    
}

// bool eep_delete_tags_with_addre(uint32_t addr){
//     uint8_t data[12];
//     eeErase(12,addr,addr+12);
//     increase_size_room
//     return true;
// }
bool find_in_epc_list(uint8_t input[]){
    for(int i = 0; i < ROOM_NUMBER ; i++){
        if(compare(input, epc_list[i])){
            Serial.println("Find in list \n");
            return true;
        }
    }
    Serial.println("Not Found\n");
    return false;
}
bool load_tag_list(){
    return false;
}
bool compare(uint8_t input[], uint8_t source[]){
    
    for (int i = 0; i < 12; i++) {
        // printf("compa: 0x%02X: 0x%02X\n", input[i], source[i] );
        if (input[i] != source[i]) {
            return false;
        } else {
            // count++;
        }
    }
    
    return true;
}
bool eep_erase_room(uint8_t index){
    uint32_t room;
    switch (index)
    {
    case 1:
      /* code */
      room = ROOM_1_LENGTH_ADDR;
      break;
    case 2:
      room = ROOM_2_LENGTH_ADDR;
      /* code */
      break;
    case 3:
      room = ROOM_3_LENGTH_ADDR;
      /* code */
      break;
      
    case 4:
      room = ROOM_4_LENGTH_ADDR;
      /* code */
      break;
    case 5:
      room = ROOM_5_LENGTH_ADDR;
      /* code */
      break;
    case 6:
      /* code */
      room = ROOM_6_LENGTH_ADDR;
      break;
    case 7:
      room = ROOM_7_LENGTH_ADDR;
      /* code */
      break;
    case 8:
      room = ROOM_8_LENGTH_ADDR;
      /* code */
      break;
      
    case 9:
      room = ROOM_9_LENGTH_ADDR;
      /* code */
      break;
    case 10:
      room = ROOM_10_LENGTH_ADDR;
      /* code */
      break;
    case 11:
      /* code */
      room = ROOM_11_LENGTH_ADDR;
      break;
    case 12:
      room = ROOM_12_LENGTH_ADDR;
      /* code */
      break;
    case 13:
      room = ROOM_13_LENGTH_ADDR;
      /* code */
      break;
      
    case 14:
      room = ROOM_14_LENGTH_ADDR;
      /* code */
      break;
    case 15:
      room = ROOM_15_LENGTH_ADDR;
      /* code */
      break;
    case 16:
      room = ROOM_16_LENGTH_ADDR;
      /* code */
      break;
    default:

      break;
    }
    Serial.printf("Erase at: %04X", room);
    uint8_t chunkSize = 64;
    eeErase(chunkSize, room, room + 100);

    //Re-write length of room
    eep.write(room + 1 ,  0x00); //=> 0x0000
    eep.write(room ,  0x00);
    return true;
}
uint16_t get_size_room(ROOM_ADDR room){
    //Serial.printf("Get size room: 0x%04X\n",room);
    uint16_t lsb = 0,msb = 0;
    uint16_t size;

    uint16_t lsb_ind = room - 0xC + 1; 
    uint16_t msb_ind = room - 0xC;
    //Serial.printf("lsb_ind: 0x%04X, msb_ind: 0x%04X\n", lsb_ind,msb_ind);
    lsb = eep.read(lsb_ind);
    msb = eep.read(msb_ind);

    size = ( (msb & 0xFF) << 8 ) | (lsb & 0x00FF);
    if(size == 0xFFFF) size = 0;
    //Serial.printf("ROOM: 0x%04X, size:  %d\n",room, size);
    return size;
}
void decrease_size_room(ROOM_ADDR room){
    uint16_t size;
    uint16_t lsb = 0,msb = 0;

    Serial.printf("Decrease room addrress: 0x%04X\n",room);

    size = get_size_room(room);
    if(size > 0){
        size = size - 1; //0x00 00
    }
    else if(size <= 0){
        size = 0;
    }
   
    
    lsb = size & 0xFF;
    msb = (size & 0xFF00) >> 8;
    Serial.printf("Size  %d lsb: 0x%02x msb: 0x%02x\n", size,lsb,msb);

    int lsb_ind = room - 0xC + 1; 
    int msb_ind = room - 0xC;
    Serial.printf("lsb_ind: 0x%04X, msb_ind: 0x%04X\n", lsb_ind,msb_ind);
    eep.write(lsb_ind,lsb);
    eep.write(msb_ind, msb);
}

void increase_size_room(ROOM_ADDR room){
    uint16_t size;
    uint16_t lsb = 0,msb = 0;

    Serial.printf("Increse room addrress: 0x%04X\n",room);

    size = get_size_room(room);
    
    size = size + 1; //0x00 00
    
    lsb = size & 0xFF;
    msb = (size & 0xFF00) >> 8;
    Serial.printf("Size  %d lsb: 0x%02x msb: 0x%02x\n", size,lsb,msb);

    int lsb_ind = room - 0xC + 1; 
    int msb_ind = room - 0xC;
    Serial.printf("lsb_ind: 0x%04X, msb_ind: 0x%04X\n", lsb_ind,msb_ind);
    eep.write(lsb_ind,lsb);
    eep.write(msb_ind, msb);
}



search_result_t search_by_address(byte input[], uint32_t startAddr, uint8_t num_of_search){
    byte _data[12] = {0x0};
    search_result_t res;
    uint32_t _startAddr = startAddr;
    //Serial.printf("Num will searched: %d\n", num_of_search);
    for(uint8_t j = 0; j < num_of_search; j++){
        // Serial.printf("j: %d Search address: 0x%04X\n",j, _startAddr);
        memset(_data, 0, 12);
        eep.read(_startAddr,_data,12);
        // for (int h = 0; h < 12; h++)
        // {
        //     Serial.printf("%02X", _data[ h], HEX);
        // }
        // Serial.println();
        if(compare(input, _data)){
            res.addr = _startAddr;
            res.res = true;
            res.index_in_room = j;
            return res;
        }
        else {
            //Next offset
            _startAddr = _startAddr + 12;
        }

        
    }
    res.res = false;
    return res;
}
search_result_t search_in_flash(uint8_t input[], ROOM_ADDR room){
    uint16_t num_of_search = get_size_room(room);
    search_result_t result;

    result = search_by_address(input, room,num_of_search);
    
    if(result.res){
        //Serial.printf("Find in ROOM 0x%04X\n", room);
    }
    else {
       // Serial.println("Not find in flash");
    }
    return result;
}

void flashInit(){

    uint8_t eepStatus = eep.begin(eep.twiClock400kHz);   //go fast!
    if (eepStatus) {
        Serial.print(F("extEEPROM.begin() failed, status = "));
        Serial.println(eepStatus);
        while (1);
    }
}
byte controlPins[] = {B00000000, 
                  B10000000,
                  B01000000,
                  B11000000,
                  B00100000,
                  B10100000,
                  B01100000,
                  B11100000,
                  B00010000,
                  B10010000,
                  B01010000,
                  B11010000,
                  B00110000,
                  B10110000,
                  B01110000,
                  B11110000 }; 

void buzzer_done() {
	Serial.println("Done!");
    EasyBuzzer.stopBeep();
    active = true;
}                
void(* resetFunc) (void) = 0; //declare reset function @ address 0

void btnLongPressHandle(){
    Serial.println("btn reset machine is pressed");
    cleanWifiStorage();
    resetFunc();
}
void btnClick(){
    Serial.println("btnClick");
    wifi_manger_en = 1;
}
void buttonHandle(void *pvParameters){
    // void *pvParameters;
    Serial.println(xPortGetCoreID());
    while(true){
        /* Always call this function in the loop for EasyBuzzer to work. */
        EasyBuzzer.update();
        // ws.cleanupClients();
        rs_btn.tick();
        delay(100);
    }
}
void readerHandle(void *pvParameters){
    Serial.println(xPortGetCoreID());
    while(true){

        if(WiFi.isConnected()){

        }
        else {
            ledState = !ledState;
            digitalWrite(ledPin, ledState);
        }
        if(_mode == NORMAL) {
            digitalWrite(LED_STASTE_PIN, HIGH);
        }
        else {
            ledMachineState = !ledMachineState;
            digitalWrite(LED_STASTE_PIN, ledMachineState);
        }
        delay(300);
    }

}

void webHandler(void *pvParameters){
    // WebSetup();
    // while(true) {
    //     delay(5000);
    // }
}
void setup()
{
    
    
    SerialRF.begin(115200, SERIAL_8N1,16,17);
    Serial.begin(115200);

    pinMode(ledPin, OUTPUT);
    pinMode(LED_STASTE_PIN,OUTPUT);

    rs_btn.setPressTicks(5000); // that is the time when LongPressStart is called
    rs_btn.attachLongPressStop(btnLongPressHandle);
    rs_btn.attachClick(btnClick);

    digitalWrite(LED_STASTE_PIN, HIGH);
    // Initialize SPIFFS
    if(!SPIFFS.begin(true)){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
    else {
      Serial.println("Access to SPIFFS success");
    }

    //Flash init 
    flashInit();


    //Load Config 
    for(int i = 0; i < ROOM_NUMBER;i++){
        tm_delay[i] = eep.read(TIMER_DELAY_ADDR + i);
        if(tm_delay[i] == 0xFF){
            Serial.printf("Delay Slot is %d \n", tm_delay[i]);
            tm_delay[i] = 1;
            eep.write(TIMER_DELAY_ADDR + i, 1);
        } 
        else Serial.printf("Delay Slot %d: %d\n", i, tm_delay[i]);
    }


    // xTaskCreatePinnedToCore

    rfc.begin();
    
    // mux.write(HIGH,0);
    // Gain = rfc.GetPaPowerLevelFrame();
    Gain = eep.read(PA_POWER);
    if(Gain > 10) Gain = 0;
    Serial.printf("Power Level: %d\n", Gain);
    // rfc.SetFHSSFrame(false);
    
    // //Setup China 1 -> 920.125M
    
    rfc.SetRegionFrame(REGION_CODE_CHN1);
    rfc.SetRfChannelFrame(0);
    Serial.println("=======================================");
    Serial.printf("Reader Gain: %d\n", rfc.GetPaPowerFrame());
    Serial.printf("Reader region: %d\n", rfc.GetRegionFrame());
    Serial.printf("Channel: %d\n", rfc.GetRfChannelFrame());
    Serial.println("====================================================");
    // for(int i = 0; i < 100; i++ ){
    //     rfc.SetRfChannelFrame(i);

    //     int channell = rfc.GetRfChannelFrame();
    //     Serial.printf("Channel working: %d\n", channell);
    // }
    
    // uint16_t addr_start = 0x80;
    // for(int i = 0; i < 16; i++ ){
    //     addr_start = addr_start + i*100;
    //     Serial.printf("#define ROOM_%d_LENGTH_ADDR 0x%04X\n",i+1, addr_start);
    //     Serial.printf("ROOM_%d_ADDRESS = 0x%04X\n",i+1, addr_start+12);
    // }

    // addr_start = 0x80;
    // for(int i = 0; i < 16; i++ ){
    //     addr_start = addr_start + i*100;
    //     Serial.printf("ROOM_%d_ADDRESS = 0x%04X\n",i+1, addr_start+12);
    // }
    //Setup China 2 -> 840.125M
    // rfc.SetRfChannelFrame(13);
    // rfc.SetRegionFrame(2);


    // int channel = rfc.GetRfChannelFrame();
    // Serial.printf("Channel working: %d\n", channel);
    // int region = rfc.GetRegionFrame();
    // Serial.printf("Region: %d\n", region);
    
    
    
    mux.write(HIGH, 0);

    active = true;
    
    EasyBuzzer.setPin(18);
    
   
    
    WebSetup();
    // test_write();

     EasyBuzzer.singleBeep(
		frequency, 	// Frequency in hertz(HZ).  
		duration, 	// Duration of the beep in milliseconds(ms). 
		buzzer_done		// [Optional] Function to call when done.
	);
    
    xTaskCreatePinnedToCore(buttonHandle, "buttonTask", 2048, NULL, 3, NULL, ARDUINO_RUNNING_CORE);
    
    xTaskCreatePinnedToCore(readerHandle, "readerTask", 2048, NULL, 4, NULL, ARDUINO_RUNNING_CORE);
    

    // xTaskCreatePinnedToCore(webHandler, "webTask", 4000, NULL, 2, NULL, ARDUINO_RUNNING_CORE);

    vTaskStartScheduler();


}
void uhf_read_user_data(){
    
}

void t2Callback() {
    Serial.print("t2: ");
    Serial.println(millis());
  
}


bool verify_task (uint8_t input[]) 
{   uint8_t i;
    uint8_t count = 0;
    for ( i = 0; i < 12; i++) {
        if(input[i] == temp_epcp[i]) {
            count++;
        }
    }
    Serial.printf("Count: %d\n", count);
    if(count == 12) {
        Serial.println("Old tags\n");
        return true;
    }
    Serial.println("New tags\n");
    for ( i = 0; i < 12; i++){
        temp_epcp[i] = input[i];
    }
    return false;
}
void uhf_process(){
    // while (SerialRF.available())
    //     rfc.encode(SerialRF.read());

    Inventory_t label = rfc.GetLabelOnce();
    if (!rfc.error.isValid() && rfc.inventory.isValid())
    {
        Serial.printf("RSSI: %d dB PC: %02X CRC: %02X\n", (int8_t)label.RSSI,label.PC,label.CRC);
        Serial.print("EPC : ");
        for (int i = 0; i < 12; i++)
        {
            Serial.printf("%02X", label.epc[i], HEX);
        }
        Serial.println();
        EasyBuzzer.singleBeep(
                        frequency, 	// Frequency in hertz(HZ).  
                        duration, 	// Duration of the beep in milliseconds(ms). 
                        buzzer_done
                    );
        delay(500);

        if(verify_task(label.epc)) {
            delay(1000);
            return;
        }
        
        if(_mode == NORMAL){
            //Mode normal
            search_result_t res;
            for(int i = 0; i < ROOM_NUMBER; i++){
                res = search_in_flash(label.epc,table[i]);
                if(res.res) {
                    Serial.printf("Find in Room address 0x%04X, i: %d\n", table[i],i);
                    //TODO: On Relay 
                    EasyBuzzer.singleBeep(
                        frequency, 	// Frequency in hertz(HZ).  
                        duration, 	// Duration of the beep in milliseconds(ms). 
                        buzzer_done
                    );
                    delay(300);
                    EasyBuzzer.stopBeep();
                    // delay(tm_delay[i]*1000);
                    // onLed(table[i]);
                    onRelay(table[i],tm_delay[i]);
                    break;
                }
                break;
            }
            if(!res.res){
                delay(1000);
            }
            else {
                delay(2000);
            }
        }
        else if(_mode == SAVE_TAGS){
            EasyBuzzer.singleBeep(
                        frequency, 	// Frequency in hertz(HZ).  
                        duration, 	// Duration of the beep in milliseconds(ms). 
                        buzzer_done
            );
            delay(1500);
            EasyBuzzer.stopBeep();
            notifyTags(label.epc);
        }
        
    }
    else
    {
       Serial.printf("Scanning....., Error Code: %X\n", rfc.error.ErrorCode());
       delay(100);
    }
}

void loop()
{   

    uhf_process();
}

 