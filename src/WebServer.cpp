#include "Arduino.h"
#include "WebServer.h"
#include "esp_uhf.h"
#include "strings.h"
#include "EasyBuzzer.h"
// Replace with your network credentials
// Search for parameter in HTTP POST request
const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";
const char* PARAM_INPUT_3 = "ip";
const char* PARAM_INPUT_4 = "gateway";

const char* ssid1 = "P-201";
const char* password1 = "1qaz2wsx";
//Variables to save values from HTML form
String ssid;
String pass;
String ip;
String gateway;

// File paths to save input values permanently
const char* ssidPath = "/ssid.txt";
const char* passPath = "/pass.txt";
const char* ipPath = "/ip.txt";
const char* gatewayPath = "/gateway.txt";

IPAddress localIP;
//IPAddress localIP(192, 168, 1, 200); // hardcoded

// Set your Gateway IP address
IPAddress localGateway;
//IPAddress localGateway(192, 168, 1, 1); //hardcoded
IPAddress subnet(255, 255, 0, 0);

// Timer variables
unsigned long previousMillis = 0;
const long interval = 10000;  // interval to wait for Wi-Fi connection (milliseconds)



device_mote  _mode = NORMAL;
const char* pw_input = "pow";
const char* epc_tag = "epc_tag";
const char* room_storage = "room";
// Set LED GPIO
const int ledPin = 2;

// Stores LED state
#define NUM_GATE 16
bool ledState = 0;
bool ledMachineState = 0;
RFC_Class::PaPower Gain = RFC_Class::PaPower::G_1550;
// uint8_t delay1=1,delay2=1,delay3=1,delay4=1,delay5=1;
uint8_t tm_delay[NUM_GATE] =  {0};
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket  ws("/ws");

String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if (ledState){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
  return String();
}

void mode_toggle(){
  const uint8_t size = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<size> json;
    json["action"] = "change_mode";
  if(_mode != NORMAL){
    _mode = NORMAL;
    json["mode"] = "Normal";
    digitalWrite(LED_STASTE_PIN, HIGH);
  }
  else {
    _mode = SAVE_TAGS;
    json["mode"] = "Save";
    digitalWrite(LED_STASTE_PIN, LOW);
  }

    char data[100];
    size_t len = serializeJson(json, data);
    Serial.printf("data send to web %s\n", data);
    ws.textAll(data, len);
}

byte nibble(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';

  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;

  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;

  return 0;  // Not a valid hexadecimal character
}
void hexCharacterStringToBytes(byte *byteArray, const char *hexString)
{
  bool oddLength = strlen(hexString) & 1;

  byte currentByte = 0;
  byte byteIndex = 0;

  for (byte charIndex = 0; charIndex < strlen(hexString); charIndex++)
  {
    bool oddCharIndex = charIndex & 1;

    if (oddLength)
    {
      // If the length is odd
      if (oddCharIndex)
      {
        // odd characters go in high nibble
        currentByte = nibble(hexString[charIndex]) << 4;
      }
      else
      {
        // Even characters go into low nibble
        currentByte |= nibble(hexString[charIndex]);
        byteArray[byteIndex++] = currentByte;
        currentByte = 0;
      }
    }
    else
    {
      // If the length is even
      if (!oddCharIndex)
      {
        // Odd characters go into the high nibble
        currentByte = nibble(hexString[charIndex]) << 4;
      }
      else
      {
        // Odd characters go into low nibble
        currentByte |= nibble(hexString[charIndex]);
        byteArray[byteIndex++] = currentByte;
        currentByte = 0;
      }
    }
  }
}

void dumpByteArray(const byte * byteArray, const byte arraySize)
{

for (int i = 0; i < arraySize; i++)
{
  Serial.print("0x");
  if (byteArray[i] < 0x10)
    Serial.print("0");
  Serial.print(byteArray[i], HEX);
  Serial.print(", ");
}
Serial.println();
}


//handle
//TODO: save tags
void hanle_save_tags(DynamicJsonDocument json){
  
  unsigned int _size;
  String s_epc = json["epc"].as<String>();;
  // Serial.println("Crash ne: " + s_epc);
  
  byte byteArray[12] = {0};
  hexCharacterStringToBytes(byteArray, s_epc.c_str());
  // dumpByteArray(byteArray, 12);

  bool res;
  res =  eep_save_tags(json["room"].as<int>(), byteArray);

  const uint8_t size = JSON_OBJECT_SIZE(2);
  StaticJsonDocument<size> msg;
  msg["action"] = "save_tags";
  msg["status"] = res;
  char data[100];
  size_t len = serializeJson(msg, data);
  Serial.printf("data send to web %s\n", data);
  ws.textAll(data, len);
}
uint8_t get_gain(){
  return Gain;
}
void set_gain_handle(DynamicJsonDocument json){
   Gain = (RFC_Class::PaPower)json["gain"].as<int>();
   Serial.printf("Set gain: %d", Gain);
   rfc.SetPaPowerFrame(Gain);
   uint16_t _Gain = rfc.GetPaPowerFrame();
   Serial.printf("Power: %d\n", _Gain);
   const uint8_t size = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<size> msg;
    msg["action"] = "set_gain";
    msg["status"] = true;
    char data[100];
    size_t len = serializeJson(msg, data);
    Serial.printf("data send to web %s\n", data);
    ws.textAll(data, len);
}
void set_delay(DynamicJsonDocument json){
  //  delay1 = json["value"][0].as<int>();
  //  delay2 = json["value"][1].as<int>();
  //  delay3 = json["value"][2].as<int>();
  //  delay4 = json["value"][3].as<int>();
  //  delay5 = json["value"][4].as<int>();
   int num = json["value"].size();
   for(int i = 0; i < num; i++){
     tm_delay[i] = json["value"][i].as<int>();
     eep.write(TIMER_DELAY_ADDR + i,tm_delay[i]);
     Serial.printf("Set OpenDelay: %d = %d\n", tm_delay[i], json["value"][i].as<int>());

     
   }

   const uint8_t size = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<size> msg;
    msg["action"] = "delay";
    msg["status"] = true;
    char data[100];
    size_t len = serializeJson(msg, data);
    Serial.printf("data send to web %s\n", data);
    ws.textAll(data, len);
}
void handle_get_delay(){
    const uint16_t size = JSON_OBJECT_SIZE(20);
    StaticJsonDocument<size> json;
    json["action"] = "delay_get";
    for(int i = 0; i < ROOM_NUMBER;i++){
      json["value"][i] = tm_delay[i];
    }
    // json["value"][0] = delay1;
    // json["value"][1] = delay2;
    // json["value"][2] = delay3;
    // json["value"][3] = delay4;
    // json["value"][4] = delay5;
    char data[300];
    size_t len = serializeJson(json, data);
    Serial.printf("data send to web %s\n", data);
    ws.textAll(data, len);
}
//message 
void notify_led_msg(){
  const uint8_t size = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<size> json;
    json["action"] = "led_toggle";
    json["led_status"] = ledState ? "ON" : "OFF";
    char data[100];
    size_t len = serializeJson(json, data);
    Serial.printf("data send to web %s\n", data);
    ws.textAll(data, len);
}
void notify_gain_msg(){
    const uint8_t size = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<size> json;
    json["action"] = "get_gain";
    json["gain"] = Gain;
    char data[100];
    size_t len = serializeJson(json, data);
    Serial.printf("data send to web %s\n", data);
    ws.textAll(data, len);
}
void notifyInit() {
    Serial.println("Send msg init");
    const uint8_t size = JSON_OBJECT_SIZE(5);
    StaticJsonDocument<size> json;
    json["action"] = "init";
    json["led_status"] = ledState ? "ON" : "OFF";
    json["gain"] = Gain;
    json["delay"] = tm_delay[0];
    if(_mode == NORMAL){
      json["mode"] = "Normal";
    }
    else {
      json["mode"] = "Save";
    }
    char data[300];
    size_t len = serializeJson(json, data);
    Serial.printf("data send to web %s\n", data);
    ws.textAll(data, len);
}
void notifyTags(uint8_t epc[12]){
    const uint8_t size = JSON_OBJECT_SIZE(2);
    const char* nEpc;
    StaticJsonDocument<size> json;
    json["action"] = "post_epc";
    
    char output[24];

    /* pointer to the first item (0 index) of the output array */
    char *ptr = &output[0];

    int i;

    for (i = 0; i < 12; i++) {
        ptr += sprintf(ptr, "%02X", epc[i]);
    }


    printf("%s\n", output);

    String str = ""; 
    for (int j = 0; j < 24; j++) { 
        str = str + output[j]; 
    } 
    json["epc"] = str.c_str();

    char data[100];
    size_t len = serializeJson(json, data);
    Serial.printf("data send to web %s\n", data);
    ws.textAll(data, len);
}

void handle_erase_storage(DynamicJsonDocument json){
    bool res; 
    //TODO: eep_erase_room(json["room"].as<int>());
    res = eep_erase_room(json["room"].as<int>());
    const uint8_t size = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<size> msg;
    msg["action"] = "erase";
    msg["status"] = res;
    char data[100];
    size_t len = serializeJson(msg, data);
    Serial.printf("data send to web %s\n", data);
    ws.textAll(data, len);
}
void handle_get_storage(DynamicJsonDocument json){
  //TODO: eep_get_room();
    ROOM_ADDR room;
    uint16_t size = 0;
    switch (json["room"].as<int>())
    {
    case 1:
      /* code */

      room = ROOM_ADDR::ROOM_1_ADDRESS;
      break;
    case 2:
      room = ROOM_ADDR::ROOM_2_ADDRESS;
      /* code */
      break;
    case 3:
      room = ROOM_ADDR::ROOM_3_ADDRESS;
      /* code */
      break;
      
    case 4:
      room = ROOM_ADDR::ROOM_4_ADDRESS;
      /* code */
      break;
    case 5:
      room = ROOM_ADDR::ROOM_5_ADDRESS;
      break;
    case 6:
      /* code */

      room = ROOM_ADDR::ROOM_6_ADDRESS;
      break;
    case 7:
      room = ROOM_ADDR::ROOM_7_ADDRESS;
      /* code */
      break;
    case 8:
      room = ROOM_ADDR::ROOM_8_ADDRESS;
      /* code */
      break;
      
    case 9:
      room = ROOM_ADDR::ROOM_9_ADDRESS;
      /* code */
      break;
    case 10:
      room = ROOM_ADDR::ROOM_10_ADDRESS;
    case 11:
      /* code */

      room = ROOM_ADDR::ROOM_11_ADDRESS;
      break;
    case 12:
      room = ROOM_ADDR::ROOM_12_ADDRESS;
      /* code */
      break;
    case 13:
      room = ROOM_ADDR::ROOM_13_ADDRESS;
      /* code */
      break;
      
    case 14:
      room = ROOM_ADDR::ROOM_14_ADDRESS;
      /* code */
      break;
    case 15:
      room = ROOM_ADDR::ROOM_15_ADDRESS;
      /* code */
      break;
    case 16:
      room = ROOM_ADDR::ROOM_16_ADDRESS;
      /* code */
      break;  
    default:
      break;
    }
    size = get_size_room(room);
    const uint8_t _size = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<_size> msg;
    msg["action"] = "get_storage";
    msg["size"] = size;
    char data[100];
    size_t len = serializeJson(msg, data);
    Serial.printf("data send to web %s\n", data);
    ws.textAll(data, len);
}
void handle_finds_tags_storagre_index(DynamicJsonDocument json){
  unsigned int _size;
  String s_epc = json["epc"].as<String>();;
  
  
  byte byteArray[12] = {0};
  hexCharacterStringToBytes(byteArray, s_epc.c_str());
  // dumpByteArray(byteArray, 12);

  search_result_t res;
  res =  eep_find_tags(byteArray);
  Serial.printf("find tags: tags_index: %d, index_room: %d 0x%04X\n" ,res.index_in_room, res.index_room, res.addr_room);
  const uint8_t size = JSON_OBJECT_SIZE(3);
  StaticJsonDocument<size> msg;
  msg["action"] = "find_tags";
  msg["status"] = res.res;
  if(res.res) {
    msg["index_room"] = res.index_room;
  }
  


  char data[100];
    size_t len = serializeJson(msg, data);
    Serial.printf("data send to web %s\n", data);
    ws.textAll(data, len);
}
void handle_delete_tags_storagre_index(DynamicJsonDocument json){
  unsigned int _size;
  String s_epc = json["epc"].as<String>();;
  // Serial.println("Crash ne: " + s_epc);
  
  byte byteArray[12] = {0};
  hexCharacterStringToBytes(byteArray, s_epc.c_str());
  // dumpByteArray(byteArray, 12);

  bool res;
  res =  eep_delete_tags(byteArray);


  const uint8_t size = JSON_OBJECT_SIZE(2);
  StaticJsonDocument<size> msg;
  msg["action"] = "delete_tags";
  msg["status"] = res;


    char data[100];
    size_t len = serializeJson(msg, data);
    Serial.printf("data send to web %s\n", data);
    ws.textAll(data, len);

}
void notifyClients(String msg) {
  ws.textAll(msg);
}

//TODO: handle data from front-end
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {


    // const uint8_t size = JSON_OBJECT_SIZE(1);

        DynamicJsonDocument json(300);
        // StaticJsonDocument<size> json;
        DeserializationError err = deserializeJson(json, data);
        if (err) {
            Serial.print(F("deserializeJson() failed with code "));
            Serial.println(err.f_str());
            return;
        }

        const char *action = json["action"];
        
        if (strcmp(action, "toggle_led") == 0) {
            Serial.printf("Toggle led");
            ledState = !ledState;
            digitalWrite(ledPin, ledState);
            notify_led_msg();
        }
        else if(strcmp(action, "get_gain") == 0 ){
            notify_gain_msg();
        }
        else if(strcmp(action, "set_gain") == 0 ){
            Serial.printf("set pow");
            set_gain_handle(json);
            //notifyMsg();
        }
        else if(strcmp(action, "save_tags") == 0 ){
            Serial.printf("Save\n");
            //TODO Save Tags
            hanle_save_tags(json);
            //notifyMsg();
        }
        else if(strcmp(action, "change_mode") == 0){
            Serial.printf("mode toggle\n");
            mode_toggle();
        }
        else if(strcmp(action, "delay") == 0){
            Serial.printf("mode set delay time\n");
            set_delay(json);
        }
        else if(strcmp(action, "delay_get") == 0){
            handle_get_delay();
        }
        else if(strcmp(action, "erase") == 0){
            handle_erase_storage(json);
        }
        else if(strcmp(action, "get_storage") == 0){
          handle_get_storage(json);
        }
        else if(strcmp(action, "buzzer") == 0){
          EasyBuzzer.singleBeep(1000, 250, buzzer_done);
        }
        else if(strcmp(action, "find_tags") == 0){
          handle_finds_tags_storagre_index(json);
        }
        else if(strcmp(action, "delete_tags") == 0){
          handle_delete_tags_storagre_index(json);
        }
  }
}

//TODO: Receive data from front-end
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      notifyInit();
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      Serial.printf("data from web %s, len = %d\n ", data,len);
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

// Read File from SPIFFS
String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path);
  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    return String();
  }
  
  String fileContent;
  while(file.available()){
    fileContent = file.readStringUntil('\n');
    break;     
  }
  return fileContent;
}

// Write file to SPIFFS
void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- frite failed");
  }
}
bool deleteFile(fs::FS &fs, const char* path){
  Serial.printf("Remove file: %s\r\n", path);
  bool res = fs.remove(path);
  return res;
}

void cleanWifiStorage(){
    deleteFile(SPIFFS, ssidPath);
    deleteFile(SPIFFS, passPath);
    deleteFile(SPIFFS, ipPath);
    deleteFile (SPIFFS, gatewayPath);
}
// Initialize WiFi
bool initWiFi() {
  if(ssid=="" || ip==""){
    Serial.println("Undefined SSID or IP address.");
    return false;
  }

  WiFi.mode(WIFI_STA);
  localIP.fromString(ip.c_str());
  localGateway.fromString(gateway.c_str());


  if (!WiFi.config(localIP, localGateway, subnet)){
    Serial.println("STA Failed to configure");
    // cleanWifiStorage();
    return false;
  }

  WiFi.begin(ssid.c_str(), pass.c_str());

  Serial.println("Connecting to WiFi...");

  unsigned long currentMillis = millis();
  previousMillis = currentMillis;

  while(WiFi.status() != WL_CONNECTED) {
    currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      Serial.println("Failed to connect.");
      // cleanWifiStorage();
      return false;
    }
  }

  Serial.println(WiFi.localIP());

  return true;
}

void WebSetup(){

    // Connect to Wi-Fi
    // WiFi.begin(ssid1, password1);
    // while (WiFi.status() != WL_CONNECTED) {
    //   delay(1000);
    //   Serial.println("Connecting to WiFi..");
    // }

    // // Print ESP32 Local IP Address
    // Serial.println(WiFi.localIP());

    // // Route for root / web page
    // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    //   request->send(SPIFFS, "/index.html", String(), false, processor);
    // });
    // //Route to load style.css file
    // server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request){
    //   request->send(SPIFFS, "/styles.css", "text/css");
    // });

    
    // initWebSocket();
    // // Start server
    // server.begin();
    // Load values saved in SPIFFS
    ssid = readFile(SPIFFS, ssidPath);
    pass = readFile(SPIFFS, passPath);
    ip = readFile(SPIFFS, ipPath);
    gateway = readFile (SPIFFS, gatewayPath);
    Serial.println(ssid);
    Serial.println(pass);
    Serial.println(ip);
    Serial.println(gateway);

    if(initWiFi()) {
        // Route for root / web page
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", String(), false, processor);
        });
        //Route to load style.css file
        server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/styles.css", "text/css");
        });
        
        initWebSocket();

        server.begin();
    }
    else {
      // Connect to Wi-Fi network with SSID and password
      Serial.println("Setting AP (Access Point)");
      // NULL sets an open Access Point
      WiFi.softAP("WIFI-MANAGER", "123456789");

      IPAddress IP = WiFi.softAPIP();
      Serial.print("AP IP address: ");
      Serial.println(IP); 

      // Web Server Root URL
      server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/wifimanager.html", "text/html");
      });

      server.serveStatic("/", SPIFFS, "/");
      
      server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
        int params = request->params();
        for(int i=0;i<params;i++){
          AsyncWebParameter* p = request->getParam(i);
          if(p->isPost()){
            // HTTP POST ssid value
            if (p->name() == PARAM_INPUT_1) {
              ssid = p->value().c_str();
              Serial.print("SSID set to: ");
              Serial.println(ssid);
              // Write file to save value
              writeFile(SPIFFS, ssidPath, ssid.c_str());
            }
            // HTTP POST pass value
            if (p->name() == PARAM_INPUT_2) {
              pass = p->value().c_str();
              Serial.print("Password set to: ");
              Serial.println(pass);
              // Write file to save value
              writeFile(SPIFFS, passPath, pass.c_str());
            }
            // HTTP POST ip value
            if (p->name() == PARAM_INPUT_3) {
              ip = p->value().c_str();
              Serial.print("IP Address set to: ");
              Serial.println(ip);
              // Write file to save value
              writeFile(SPIFFS, ipPath, ip.c_str());
            }
            // HTTP POST gateway value
            if (p->name() == PARAM_INPUT_4) {
              gateway = p->value().c_str();
              Serial.print("Gateway set to: ");
              Serial.println(gateway);
              // Write file to save value
              writeFile(SPIFFS, gatewayPath, gateway.c_str());
            }
            //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
          }
        }
        request->send(200, "text/plain", "Cai dat xong. Vui long truy cap trang web chinh tai dia chi ip sau: " + ip);
        delay(3000);
        ESP.restart();
      });
      server.begin();
    }
}