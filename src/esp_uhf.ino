
#include <Arduino.h>
#include "RF_Commands.h"
#include "HardwareSerial.h"
#include <ios>
#include <algorithm>    // std::equal
HardwareSerial SerialRF(2);
RFC_Class rfc(&SerialRF);

#define RFIDEN 14
uint8_t epc_list[4][12] = {
    {0xE2,0x80,0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0xA8, 0x4B},
    {0xE2,0x80,0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0x6A, 0x19},
    {0xE2,0x80,0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x18},
    {0xE2, 0x80,0x68,0x94,0x00,0x00,0x50,0x1A,0x4D,0x13,0x6A,0x1B}
};


uint8_t test_source[12] = {0xE2,0x80,0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x18};
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
void setup()
{
    // pinMode(RFIDEN, OUTPUT);
    // digitalWrite(RFIDEN, 0);
    // delay(100);
    // digitalWrite(RFIDEN, 1);
    // delay(500);
    //SerialRF.pins(15,13);
    SerialRF.begin(115200, SERIAL_8N1,16,17);
    Serial.begin(115200);
    rfc.begin();

    find_in_epc_list(test_source);
     
}
void uhf_read_user_data(){
    
}
void flow(){
    Inventory_t label = rfc.GetLabelOnce();
    if (!rfc.error.isValid() && rfc.inventory.isValid())
    {
        Serial.printf("RSSI : %d dB\n", (int8_t)label.RSSI);
        Serial.printf("PC : %02X\n", label.PC);

        Serial.print("EPC : ");
        for (int i = 0; i < 12; i++)
        {
            Serial.printf("%02X", label.epc[i], HEX);
        }
        Serial.println();
        Serial.printf("CRC : %02X\n", label.CRC);
        find_in_epc_list(label.epc);
    }
    else
    {
       Serial.printf("Error Code: %X\n", rfc.error.ErrorCode());
    }
}
void loop()
{

    delay(200);
    
    // while (!SerialRF.available()){

    //     rfc.encode(SerialRF.read());
    //     continue;
    // }
    

    flow();

    
}