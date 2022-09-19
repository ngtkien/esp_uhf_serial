## Flash Management (Max 256 page - every page 128 byte)

use the first 12 bytes to store room information

|Byte 1          | Byte 2      | Other |
|----------------|-------------|-------|
| Number (MSB)   | Number(LSB) | RFU   |

# Page 0 - 2 (Room 1) (Max 99 tag)

Number address: 0x0000
| Page | Start Address | End Adress |  Size (byte)  |
|------|---------------|------------|---------------|
|  0   |     0x00C     |   0x1FF    |     500       |
|  1   |     0x200     |   0x3FF    |     512       |
|  2   |     0x400     |   0x4B0    |     176       |
# Page 3 - 5 (Room 2)
Number address: 0x0600
| Page | Start Address  | End Adress  |  Size (byte)  |
|------|----------------|------------ |---------------|
|  3   |     0x060C     |   0x07FF    |     500       |
|  4   |     0x0800     |   0x09FF    |     512       |
|  5   |     0x0A00     |   0x0AB0    |     176       |

# Page 6 - 8 (Room 3)
Number address: 0x1200
| Page | Start Address  | End Adress  |  Size (byte)  |
|------|----------------|------------ |---------------|
|  6   |     0x120C     |   0x13FF    |     500       |
|  7   |     0x1400     |   0x15FF    |     512       |
|  8   |     0x1600     |   0x16B0    |     176       |

# Page 9 - 11 (Room 4)
Number address: 0x1800
| Page | Start Address  | End Adress  |  Size (byte)  |
|------|----------------|------------ |---------------|
|  9   |     0x180C     |   0x19FF    |     500       |
|  10  |     0x1A00     |   0x1BFF    |     512       |
|  11  |     0x1C00     |   0x1CB0    |     176       |
# Page 12 - 14 (Room 5)
Number address: 0x1E00
| Page | Start Address  | End Adress  |  Size (byte)  |
|------|----------------|------------ |---------------|
|  12   |     0x1E0C     |   0x1FFF    |     500       |
|  13  |     0x2000     |   0x21FF    |     512       |
|  14  |     0x2200     |   0x22B0    |     176       |
# Page 15 - 17
# Page 18 - 20
# Page 21 - 23
# Page 24 - 26 
# Page 27 - 28
# Page 30 - 32
# Page 33 - 35
# Page 36 - 38

## DEMO
# List tag by EPC

uint8_t epc_list[4][12] = {
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0xA8, 0x4B}, -> Room 1 (address 0x0)
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0x6A, 0x19}, -> Room 2 (address 0x600)
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x18}, -> Room 3 (address 0x1200)
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x1B}  -> Room 4 (address 0x1400)
};

//thêm passwork