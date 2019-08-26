#include "Arduino.h"
#include "src/wemos_WiFi/wemos_WiFi.h"

// macros from DateTime.h 
/* Useful Constants */
#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24L)
/* Useful Macros for getting elapsed time */
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)  
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN) 
#define numberOfHours(_time_) (( _time_% SECS_PER_DAY) / SECS_PER_HOUR)
#define elapsedDays(_time_) ( _time_ / SECS_PER_DAY)  




//WebServer
#include <FS.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266Wifi.h>
#include <ArduinoJson.h>
AsyncWebServer server(80);
AsyncWebSocket ws("/");

//Houz
#include "src/HouzCore/devs.h"
#include "src/HouzCore/HouzCore.h"
#include "src/HouzCore/HouzJson.h"
HouzCore houzCore;

#include "src/HouzRfLink/HouzRfLink.h"
HouzRfLink houzLink;
#include "src/HouzUpdater/HouzUpdater.h"
HouzUpdater updater(houzLink, houzCore);


void setup(){
  Serial.begin(115200);
  Serial.println("\n\n::: WeMos Houz Server :::");
  wemosWiFi.connect("houz_server");

//fs setup
  fsInit();
  wsInit();
  webInit();

//rf setup
  houzLink.init();
  updater.init();
};

void loop(){
  wemosWiFi.update();
  uiUpdate();
  houzCore.timer();
  updater.update();
  if(houzLink.hasData()){
    handleRecevice(houzLink.getData());
  };
};

//TODO: make ui abstract
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UI Notification
bool _ui_onNotification = false;
unsigned long _ui_onNotification_end;
void uiNotify(){
  _ui_onNotification=true;
  _ui_onNotification_end=millis()+100;
  digitalWrite(LED_BUILTIN, LOW);  
}
void uiUpdate(){
  if(!_ui_onNotification) return;
  if(millis()<_ui_onNotification_end) return;
  digitalWrite(LED_BUILTIN, HIGH);  
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FileSystem
void fsInit(){
  if(SPIFFS.begin()){
    server.serveStatic("/", SPIFFS, "/index.html");
    Serial.println("SPIFFS\tloaded");
  }else{
    Serial.println("An Error has occurred while mounting SPIFFS");
  }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sockets Server
#pragma region SocketsServer
void wsInit(){
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len){
  if(type==WS_EVT_DATA){
      String msg = "";
      //get data
      AwsFrameInfo *info = (AwsFrameInfo*)arg;
      if (!(info->final && info->index == 0 && info->len == len)){
        Serial.print("WTF?? ignoring");
        return;
      } 
      if (info->opcode == WS_TEXT) {
        for (size_t i = 0; i < info->len; i++) msg += (char)data[i];
      }
      else {
        char buff[3];
        for (size_t i = 0; i < info->len; i++) msg += buff;
      }
      Serial.print(client->id());
      Serial.printf("] socket.in> %s\n", msg.c_str());
      handleSend(msg, client->id());
      return;
  }

  switch (type) {
    case WS_EVT_CONNECT:
      ws.text(client->id(), houzCore.json_getDeviceList());
      break;

    case WS_EVT_DISCONNECT:
      break;
  }
}
#pragma endregion


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WebServer
void webInit(){
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.onNotFound(onNotFound);
  server.begin();
}

void onNotFound(AsyncWebServerRequest *request){
    request->send(404, "text/plain", "Houz :: Not found");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SocketsHandler
void handleSend(String msg, int clientId){
  DevicePkt dev = JSON.parseDevicePkt(msg);
  houzLink.send(dev);
  handleServerAct(action_ack);
};

void handleRecevice(Device dev){
  uiNotify();
  if(dev.id!=server_node)
    houzCore.updateDevice(dev);

  // build json /////
  String msg = "{";
  
  //act
  msg +="\"act\":";
  msg += dev.id==server_node?
    dev.payload //event
    :4;         //or deviceUpdate

  //device
  if(dev.id!=server_node){
    msg +=",\"dev\":";
    msg += houzCore.json_getDevice(dev.id);
  }

  msg += "}";

  //broadcast
  ws.textAll(msg);
}

void handleServerAct(int action){
//build json
  String msg = "{\"act\":";
  msg += action;
  msg += "}";

//deliver
  ws.textAll(msg);
}
