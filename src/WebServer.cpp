#include "Arduino.h"
#include "WebServer.h"
// Replace with your network credentials
const char* ssid = "P-201";
const char* password = "1qaz2wsx";
// const char* ssid = "TruyenBeo";
// const char* password = "1qaz2wsx";
enum device_mote{
  NORMAL = 0,
  SAVE_TAGS
};

device_mote  _mode = NORMAL;
const char* pw_input = "pow";
const char* epc_tag = "epc_tag";
const char* room_storage = "room";
// Set LED GPIO
const int ledPin = 2;
// Stores LED state
bool ledState = 0;
uint8_t Gain = 15;
uint8_t TrayOpen = 1;
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
  if(_mode == NORMAL){
    _mode = SAVE_TAGS;
    json["mode"] = "Normal";
  }
  else {
    _mode = NORMAL;
    json["mode"] = "Save";
  }

    char data[100];
    size_t len = serializeJson(json, data);
    Serial.printf("data send to web %s\n", data);
    ws.textAll(data, len);
}
String get_power(){
  return "26";
}

//handle
void set_gain_handle(DynamicJsonDocument json){
   Gain  = json["gain"].as<int>();
   Serial.printf("Set gain: %d", Gain);
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
  }
}

//TODO: Receive data from front-end
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
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

void WebSetup(){
    

    // Connect to Wi-Fi
    // WiFi.begin(ssid, password);
    // while (WiFi.status() != WL_CONNECTED) {
    //   delay(1000);
    //   Serial.println("Connecting to WiFi..");
    // }

    // Print ESP32 Local IP Address
    Serial.println(WiFi.localIP());

    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/index.html", String(), false, processor);
    });
    //Route to load style.css file
    server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/styles.css", "text/css");
    });

    
    initWebSocket();
    // Start server
    server.begin();
}