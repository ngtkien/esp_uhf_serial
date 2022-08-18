
#include <Arduino.h>
#include "esp_uhf.h"
// #include "RF_Commands.h"
// #include "HardwareSerial.h"
// #include <Wire.h>
// #include <extEEPROM.h>
// #include <Mux.h>
// #include "WebServer.h"
using namespace admux;

#define ROOM_SIZE 5
/*
 * Creates a Mux instance.
 *
 * 1st argument is the SIG (signal) pin (Arduino digital output pin 3).
 * 2nd argument is the S0-S3 (channel control) pins (Arduino pins 8, 9, 10, 11).
 */
Mux mux(Pin(14, OUTPUT, PinType::Digital), Pinset(25, 33,32, 27));

//Multiplex
#define EN 26
#define S0 25
#define S1 33
#define S2 32
#define S3 27


HardwareSerial SerialRF(2);
RFC_Class rfc(&SerialRF);
const uint32_t totalKBytes = 32;         //for read and write test functions
extEEPROM eep(kbits_256, 1, 64);         //device size, number of devices, page size
extern uint8_t Gain;
#define RFIDEN 14
byte epc_list[4][12] = {
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0xA8, 0x4B},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0x6A, 0x19},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x18},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x1B}
};


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

ROOM_ADDR table[ROOM_SIZE] = {
    ROOM_ADDR::ROOM_1_ADDRESS, 
    ROOM_ADDR::ROOM_2_ADDRESS, 
    ROOM_ADDR::ROOM_3_ADDRESS, 
    ROOM_ADDR::ROOM_4_ADDRESS,
    ROOM_ADDR::ROOM_5_ADDRESS
};
byte test_source[12] = {0xE2,0x80,0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x18};
byte data[12] = {0x0};


void onLed(ROOM_ADDR room){
    switch (room)
    {
    case ROOM_ADDR::ROOM_1_ADDRESS:
        delay(delay1*1000);
        mux.write(LOW, 1);
        delay(1000);
        mux.write(HIGH, 1);
        /* code */
        break;
    case ROOM_ADDR::ROOM_2_ADDRESS:
        delay(delay2*1000);
        mux.write(LOW, 2);
        delay(1000);
        mux.write(HIGH, 2);
        /* code */
        break;
    case ROOM_ADDR::ROOM_3_ADDRESS:
        delay(delay3*1000);
        mux.write(LOW, 3);
        delay(1000);
        mux.write(HIGH, 3);
        /* code */
        break;
    case ROOM_ADDR::ROOM_4_ADDRESS:
        delay(delay4*1000);
        mux.write(LOW, 4);
        delay(1000);
        mux.write(HIGH, 4);
        /* code */
        break;
    case ROOM_ADDR::ROOM_5_ADDRESS:
        delay(delay5*1000);
        mux.write(LOW, 5);
        delay(1000);
        mux.write(HIGH, 5);
        /* code */
        break;
    default:
        break;
    }
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
bool eep_save_tags(uint8_t room_number, uint8_t epc[]){

    for(int i = 0; i < 5; i++){
        bool res = search_in_flash(epc,table[i]);
        if(res) {
            Serial.printf("Find it in Room address 0x%04X\n", table[i]);
            return false;
        }
    }
    if(room_number > 5){
        Serial.println("Room not exists");
        return false;
    }
    //Get room_number -> room address and room length
    //Calculator index 
    ROOM_ADDR room = table[room_number-1];
    uint16_t size = get_size_room(room);
    uint32_t index = room;
    if(size == 0){
        index = room + 12;
    }
    else {
        index = room + (size-1)*12 + 12;
    }
    Serial.printf("Save in addr: 0x%04X room number: %d room addr: 0x%04X\n",index, room_number,room);
    //TODO: Write in the next last index
    eep.write(index,epc,12);
    //TODO: Increse length
    increase_size_room(room);
    return true;
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
bool find_in_epc_list(uint8_t input[]){
    for(int i = 0; i < ROOM_SIZE ; i++){
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
        //printf("compa: 0x%02X: 0x%02X\n", input[i], source[i] );
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
    default:

      break;
    }
    Serial.printf("Erase at: %04X", room);
    uint8_t chunkSize = 64;
    eeErase(chunkSize, room, room + 0x4B0);

    //Re-write length of room
    eep.write(room + 1 ,  0x00); //=> 0x0000
    eep.write(room ,  0x00);
    return true;
}
uint16_t get_size_room(ROOM_ADDR room){
    uint16_t lsb = 0,msb = 0;
    uint16_t size;
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
    case ROOM_ADDR::ROOM_5_ADDRESS:
        lsb = eep.read(ROOM_5_LENGTH_ADDR + 1);
        msb = eep.read(ROOM_5_LENGTH_ADDR);
        break;
    default:
        Serial.println("ROOM Address not exist");
        break;
    }
    size = ( (msb & 0xFF) << 8 ) | (lsb & 0x00FF);
    if(size == 0xFFFF) size = 0;
    Serial.printf("Number of tag store in ROOM %d\n", size);
    return size;
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
    switch (room)
    {
    case ROOM_ADDR::ROOM_1_ADDRESS:
        eep.write(ROOM_1_LENGTH_ADDR + 1,lsb);
        eep.write(ROOM_1_LENGTH_ADDR, msb);
        /* code */
        break;
    case ROOM_ADDR::ROOM_2_ADDRESS:
        eep.write(ROOM_2_LENGTH_ADDR + 1,lsb);
        eep.write(ROOM_2_LENGTH_ADDR, msb);
        break;
    case ROOM_ADDR::ROOM_3_ADDRESS:
        eep.write(ROOM_3_LENGTH_ADDR + 1,lsb);
        eep.write(ROOM_3_LENGTH_ADDR, msb);
        break;
    case ROOM_ADDR::ROOM_4_ADDRESS:
        eep.write(ROOM_4_LENGTH_ADDR + 1,lsb);
        eep.write(ROOM_4_LENGTH_ADDR, msb);
        break;
    case ROOM_ADDR::ROOM_5_ADDRESS:
        eep.write(ROOM_5_LENGTH_ADDR + 1,lsb);
        eep.write(ROOM_5_LENGTH_ADDR, msb);
        break;
    default:
        Serial.println("ROOM Address not exist");
        break;
    }
}
bool search_by_address(byte input[], uint32_t startAddr, uint8_t num_of_search){
    byte _data[12] = {0x0};
    uint32_t _startAddr = startAddr;
    //Serial.printf("Num will searched: %d\n", num_of_search);
    for(uint8_t j = 0; j < 10; j++){
        //Serial.printf("j: %d Search at: 0x%04X\n",j, startAddr);
        memset(_data, 0, 12);
        eep.read(_startAddr,_data,12);
        if(compare(input, _data)){
            return true;
        }
        else {
            //Next offset
            _startAddr += 12;
        }

        
    }
    return false;
}
bool search_in_flash(uint8_t input[], ROOM_ADDR room){
    uint16_t num_of_search = get_size_room(room);
    bool result = false;
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
    case ROOM_ADDR::ROOM_5_ADDRESS:
        result = search_by_address(input, ROOM_ADDR::ROOM_5_ADDRESS,num_of_search);    
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
void setup()
{
    
    
    SerialRF.begin(115200, SERIAL_8N1,16,17);
    Serial.begin(115200);

    pinMode(ledPin, OUTPUT);
    // Initialize SPIFFS
    if(!SPIFFS.begin(true)){
      Serial.println("An Error has occurred while mounting SPIFFS");
      return;
    }
    else {
      Serial.println("Access to SPIFFS success");
    }
    
    WebSetup();

    rfc.begin();
    
    Gain = rfc.GetPaPowerLevelFrame();
    Serial.printf("Power Level: %d\n", Gain);
    flashInit();

    // test_write();
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

        if(_mode == NORMAL){
            //Mode normal
            bool res = false;
            for(int i = 0; i < 5; i++){
                res = search_in_flash(label.epc,table[i]);
                if(res) {
                    Serial.printf("Find it in Room address 0x%04X\n", table[i]);
                    //TODO: On Relay 
                    //
               
                    onLed(table[i]);
                    break;
                }
            }
        }
        else if(_mode == SAVE_TAGS){
            notifyTags(label.epc);
        }
        
    }
    else
    {
       Serial.printf("Error Code: %X\n", rfc.error.ErrorCode());
    }
}

void loop()
{

    delay(300);
    //delay(500);
    
    uhf_process();

}

 