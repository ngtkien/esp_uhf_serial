
#include <Arduino.h>
#include "RF_Commands.h"
#include "HardwareSerial.h"
#include <Wire.h>
#include <AT24Cxx.h>

#define i2c_address 0x50
/** the current address in the EEPROM (i.e. which byte we're going to write to next) **/
int address = 32767;

AT24Cxx eep(i2c_address, 32);
HardwareSerial SerialRF(2);
RFC_Class rfc(&SerialRF);

#define RFIDEN 14
uint8_t epc_list[4][12] = {
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0xA8, 0x4B},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0x6A, 0x19},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x18},
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x1B}
};

#define ROOM_NUMBER 4

uint8_t room1[10][12] = {
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0xA8, 0x4B},
};
uint8_t room2[10][12] = {
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0x6A, 0x19}, 
};
uint8_t room3[10][12] = {
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x18},
};
uint8_t room4[10][12] = {
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x1B}
};

uint8_t test_source[12] = {0xE2,0x80,0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x18};

void test_eeprom(){
    int val = analogRead(0) / 4;
	Serial.println(val);

	eep.write(address, 'c');
	/***
	Advance to the next address, when at the end restart at the beginning.
	***/
	address = address + 1;
	if (address == eep.length())
	{
        Serial.println("reset address");
		address = 32767;
	}

    // read a byte from the current address of the EEPROM
    byte value = eep.read(address);

    Serial.print(address);
    Serial.print("\t");
    Serial.print(value, DEC);
    Serial.println();

    address = address + 1;
    if (address == eep.length()) {
        Serial.println("reset address");
        address = 0;
    }

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

void printContents();
void setup()
{
    
    SerialRF.begin(115200, SERIAL_8N1,16,17);
    Serial.begin(115200);
    rfc.begin();
    Wire.begin();
    //find_in_epc_list(test_source);
    Serial.println ( "14CORE | 24C256 EEPROM TEST CODE"); // send a message to the serial monitor 	
    Serial.println ( "--------------------------------");
    Serial.println ( "START... ");
    Wire.begin();
    //
    test_eeprom();
    printContents();
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
void test_flash(){
    while (Serial.available()) {
    int i, m;
    String data = Serial.readStringUntil('\n');
 
    for (i = 0, m = data.length(); i < m; ++i) {
      eep.write(i, data[i]);
	  
	  Serial.println("14CORE | AT24C256 WRITE CODE TEST");
	  Serial.println("---------------------------------");
      Serial.println("Writing  : ");
      Serial.print(data[i]);
      Serial.println(" IN : ");
      Serial.println(i);
    }
    eep.write(i, 0xFF); //Start write 
    printContents();
  }
  delay(1);
}
void loop()
{

    delay(300);
    uhf_process();


    //Test Flash
    //test_flash();
}


 
void printContents() {
int eeaddress = 32767;
byte value;
 
  while (true) {
    value = eep.read(eeaddress++);
    if ((value == 0xFF) || (eeaddress == 32768)) {
      Serial.println();
      break;
    }
    Serial.print((char)value);
  }
}
 