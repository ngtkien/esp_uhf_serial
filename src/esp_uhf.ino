
#include <Arduino.h>
#include "RF_Commands.h"
#include "HardwareSerial.h"
#include <Wire.h>
#include <extEEPROM.h>


HardwareSerial SerialRF(2);
RFC_Class rfc(&SerialRF);
const uint32_t totalKBytes = 32;         //for read and write test functions
extEEPROM eep(kbits_256, 1, 64);         //device size, number of devices, page size

#define RFIDEN 14
byte epc_list[4][12] = {
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0xA8, 0x4B},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0x6A, 0x19},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x18},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x1B}
};

typedef enum {
  ROOM_1_ADDRESS = 0x0C,
  ROOM_2_ADDRESS = 0x60C,
  ROOM_3_ADDRESS = 0x120C,
  ROOM_4_ADDRESS = 0x180C,
} ROOM_ADDR;
#define ROOM_NUMBER 4
#define ROOM_1_LENGTH_ADDR 0x0
byte room1[10][12] = {
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0xA8, 0x4B},
};
#define ROOM_2_LENGTH_ADDR 0x600

byte room2[10][12] = {
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0x6A, 0x19}, 
};
#define ROOM_3_LENGTH_ADDR 0x1200

byte room3[10][12] = {
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x18},
};
#define ROOM_4_LENGTH_ADDR 0x1800

byte room4[10][12] = {
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x1B}
};

byte test_source[12] = {0xE2,0x80,0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x18};
byte data[12] = {0x0};

void load_data(){

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
void test_eeprom(){
    Serial.println("TEST WRITE");

    eep.write(ROOM_1_LENGTH_ADDR + 1 ,  0x1); //=> 0x1001
    eep.write(ROOM_1_LENGTH_ADDR ,  0x00);
    eep.write(ROOM_ADDR::ROOM_1_ADDRESS,epc_list[0],12);
    eep.write(ROOM_2_LENGTH_ADDR + 1 ,  1);
    eep.write(ROOM_ADDR::ROOM_2_ADDRESS,epc_list[1],12);
    eep.write(ROOM_3_LENGTH_ADDR + 1 ,  1);
    eep.write(ROOM_ADDR::ROOM_3_ADDRESS,epc_list[2],12);
    eep.write(ROOM_4_LENGTH_ADDR + 1 ,  1);
    eep.write(ROOM_ADDR::ROOM_4_ADDRESS,epc_list[3],12);
    Serial.println("TEST READ");

    uint8_t maxDataSize = 12;
    memset(data, 0, maxDataSize);
    eep.read(ROOM_1_ADDRESS,data,maxDataSize);
    for(int i = 0; i < maxDataSize ; i++){
        Serial.printf("%02X", data[i], HEX);
    }
    Serial.println();

    memset(data, 0, maxDataSize);
    eep.read(ROOM_2_ADDRESS,data,maxDataSize);
    for(int i = 0; i < maxDataSize ; i++){
        Serial.printf("%02X", data[i], HEX);
    }
    Serial.println();

    memset(data, 0, maxDataSize);
    eep.read(ROOM_3_ADDRESS,data,maxDataSize);
    for(int i = 0; i < maxDataSize ; i++){
        Serial.printf("%02X", data[i], HEX);
    }
    Serial.println();

    memset(data, 0, maxDataSize);
    eep.read(ROOM_4_ADDRESS,data,maxDataSize);
    for(int i = 0; i < maxDataSize ; i++){
        Serial.printf("%02X", data[i], HEX);
    }
    Serial.println();
}
bool find_in_epc_list(uint8_t input[]){
    for(int i = 0; i < 4 ; i++){
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
    int count = 0;
    for (int i = 0; i < 12; i++) {
        if (input[i] != source[i]) {

        } else {
             count++;
        }
    }
    return count == 12;
}
uint16_t get_size_room(ROOM_ADDR room){
    uint16_t lsb = 0,msb = 0;
    switch (room)
    {
    case ROOM_ADDR::ROOM_1_ADDRESS:
        lsb = eep.read(ROOM_1_LENGTH_ADDR + 1);
        msb = eep.read(ROOM_1_LENGTH_ADDR);
        /* code */
        break;
    case ROOM_ADDR::ROOM_2_ADDRESS:
        lsb = eep.read(ROOM_2_LENGTH_ADDR + 1);
        msb = eep.read(ROOM_2_LENGTH_ADDR);
        break;
    case ROOM_ADDR::ROOM_3_ADDRESS:
        lsb = eep.read(ROOM_3_LENGTH_ADDR + 1);
        msb = eep.read(ROOM_3_LENGTH_ADDR);
        break;
    case ROOM_ADDR::ROOM_4_ADDRESS:
        lsb = eep.read(ROOM_4_LENGTH_ADDR + 1);
        msb = eep.read(ROOM_4_LENGTH_ADDR);
        break;
    default:
        break;
    }
    Serial.printf("msb: %02X, lsb: %02X", msb, lsb);
    return ( (msb & 0xFF) << 8 ) | (lsb & 0x00FF);
}
bool search_by_address(byte input[], uint32_t start, uint32_t num_of_search = 40){
    byte _data[12] = {0x0};
    uint32_t startAddr = start;
    for(uint8_t i = 0; i < num_of_search; i++){
        memset(_data, 0, 12);
        eep.read(startAddr,_data,12);
        if(compare(input, _data)){
            return true;
        }
        else {
            //Next offset
            startAddr += 12;
        }
    }
}
bool search_in_flash(uint8_t input[], uint8_t lenght, ROOM_ADDR room){
    uint16_t num_of_search = get_size_room(room);
    bool result;
    switch (room)
    {
    case ROOM_ADDR::ROOM_1_ADDRESS:
        result =  search_by_address(input, ROOM_ADDR::ROOM_1_ADDRESS,num_of_search);
        /* code */
        break;
    case ROOM_ADDR::ROOM_2_ADDRESS:
        result =  search_by_address(input, ROOM_ADDR::ROOM_2_ADDRESS,num_of_search);
        break;
    case ROOM_ADDR::ROOM_3_ADDRESS:
        result = search_by_address(input, ROOM_ADDR::ROOM_3_ADDRESS,num_of_search);
        break;
    case ROOM_ADDR::ROOM_4_ADDRESS:
        result = search_by_address(input, ROOM_ADDR::ROOM_4_ADDRESS,num_of_search);
        break;
    default:
        break;
    }
    if(result){
        Serial.printf("Find in ROOM 0x%04X\n", room);
    }
    else {
        Serial.println("Not find in flash");
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

void setup()
{
    
    SerialRF.begin(115200, SERIAL_8N1,16,17);
    Serial.begin(115200);
    rfc.begin();
    flashInit();

    //erase all
    //uint8_t chunkSize = 64;    //this can be changed, but must be a multiple of 4 since we're writing 32-bit integers
    //eeErase(chunkSize, 0, totalKBytes * 1024 - 1);


    //test_eeprom();
    uint16_t size = get_size_room(ROOM_ADDR::ROOM_1_ADDRESS);
    Serial.printf("Room size %d", size);
    search_in_flash(test_source,0,ROOM_ADDR::ROOM_4_ADDRESS);
}
void uhf_read_user_data(){
    
}
void uhf_process(){
    Inventory_t label = rfc.GetLabelOnce();
    if (!rfc.error.isValid() && rfc.inventory.isValid())
    {
        Serial.printf("RSSI : %d dB\nPC : %02X\nCRC : %02X\n", (int8_t)label.RSSI,label.PC,label.CRC);
        Serial.print("EPC : ");
        for (int i = 0; i < 12; i++)
        {
            Serial.printf("%02X", label.epc[i], HEX);
        }
        Serial.println();
        find_in_epc_list(label.epc);
    }
    else
    {
       Serial.printf("Error Code: %X\n", rfc.error.ErrorCode());
    }
}

void loop()
{

    delay(300);
    // uhf_process();

}

 