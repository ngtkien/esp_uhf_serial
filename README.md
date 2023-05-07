## Flash Management (Max 256 page - every page 128 byte)

## DEMO
# List tag by EPC

uint8_t epc_list[4][12] = {
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0xA8, 0x4B}, -> Room 1 (address 0x0)
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x40, 0x1A, 0x4D, 0x13, 0x6A, 0x19}, -> Room 2 (address 0x600)
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x18}, -> Room 3 (address 0x1200)
    {0xE2, 0x80, 0x68, 0x94, 0x0,0x0, 0x50, 0x1A, 0x4D, 0x13, 0x6A, 0x1B}  -> Room 4 (address 0x1400)
};


## Function
### WIFI

#### AP STATE
- Khi không kết nối được với wifi -> ESP32 ở trạng thái AP. Cung cấp người dùng 1 trang WIFI_MANAGER để đăng nhập

#### CLIETN STATE (recommend)
- Khi được kết nối với wifi -> WIfi ESP32 hoạt động ở trạng thái Client, cung cấp cho người dùng một trang devices manager để thay đổi config devices  

###  UHF Reader
- Luôn luôn hoạt động bất kể WIFI đang ở trạng thái nào. ( phiên bản cũ chỉ hoạt động khi wifi ở trạng thái Client)

### Update   
  
  Lưu các trạng thái của devices vào flash. Đọc lại khi device được RESET
    - Delay time của tứng slot. 
    - Khoảng cách đọc của đầu đọc ( get trực tiếp từ đầu đọc khi devices RESET )