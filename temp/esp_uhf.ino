
/*
 * @Author: your name
 * @Date: 2021-10-29 09:40:02
 * @LastEditTime: 2021-10-29 09:50:14
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \MagicRF M100\example\SimpleSetting\SimpleSetting.ino
 */
#include <Arduino.h>
#include "RF_Commands.h"
#include "HardwareSerial.h"

HardwareSerial SerialRF(2);
RFC_Class rfc(&SerialRF);

#define RFIDEN 14

void setup()
{
    pinMode(RFIDEN, OUTPUT);
    digitalWrite(RFIDEN, 0);
    delay(100);
    digitalWrite(RFIDEN, 1);
    delay(500);

    SerialRF.begin(115200, SERIAL_8N1,16,17);
    Serial.begin(115200);
    rfc.begin();

    Serial.println(rfc.SetComBaudrate(0x0480) ? "SUCCESS" : "Fail");           // 115200
    Serial.println(rfc.SetPaPowerFrame(0) ? "SUCCESS" : "Fail");          // 20dbm
    Serial.println(rfc.SetRegionFrame(REGION_CODE_CHN1) ? "SUCCESS" : "Fail"); // China 900MHz
    Serial.println(rfc.SetRfChannelFrame(0x01) ? "SUCCESS" : "Fail");          // If it is the 900MHz frequency band in China, set the working channel of the reader to 920.375MHz
}

void loop()
{
    delay(200);
    while (SerialRF.available())
        rfc.encode(SerialRF.read());

    Inventory_t label = rfc.GetLabelOnce();
    if (!rfc.error.isValid() && rfc.inventory.isValid())
    {
        Serial.printf("RSSI : %d dB\n", (int8_t)label.RSSI);
        Serial.printf("PC : %02X\n", label.PC);

        Serial.print("EPC : ");
        for (int i = 0; i < 12; i++)
        {
            Serial.print(label.epc[i], HEX);
        }
        Serial.println();
        Serial.printf("CRC : %02X\n", label.CRC);
    }
    else
    {
        Serial.printf("Error Code: %X\n", rfc.error.ErrorCode());
    }
}

//