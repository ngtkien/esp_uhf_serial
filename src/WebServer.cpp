#include "Arduino.h"
#include "WebServer.h"
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
bool ledState = 0;
uint8_t Gain = 15;
uint8_t OpenDelay = 1;
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
  }
  else {
    _mode = SAVE_TAGS;
    json["mode"] = "Save";
  }

    char data[100];
    size_t len = serializeJson(json, data);
    Serial.printf("data send to web %s\n", data);
    ws.textAll(data, len);
}


//handle
uint8_t get_gain(){
  return Gain;
}
void set_gain_handle(DynamicJsonDocument json){
   Gain  = json["gain"].as<int>();
   Serial.printf("Set gain: %d", Gain);
}
void set_delay(DynamicJsonDocument json){
   OpenDelay = json["value"].as<int>();
   Serial.printf("Set OpenDelay: %d", OpenDelay);
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
    json["delay"] = OpenDelay;
    if(_mode == NORMAL){
      _mode = SAVE_TAGS;
      json["mode"] = "Normal";
    }
    else {
      _mode = NORMAL;
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
void notifyClients(String msg) {
  ws.textAll(msg);
}

//TODO: handle data from front-end
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {


    // const uint8_t size = JSON_OBJECT_SIZE(1);

        DynamicJsonDocument json(100);
        // StaticJsonDocument<size> json;
        DeserializationError err = deserializeJson(json, data);
        if (err) {
            Serial.print(F("deserializeJson() failed with code "));
            Serial.println(err.c_str());
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
      Serial.printf("data from web %s\n", data);
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
      WiFi.softAP("WIFI-MANAGER", NULL);

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
        request->send(200, "text/plain", "Done. ESP will restart, connect to your router and go to IP address: " + ip);
        delay(3000);
        ESP.restart();
      });
      server.begin();
    }
}