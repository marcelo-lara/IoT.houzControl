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
HouzCore houzCore;

#include "src/HouzRfLink/HouzRfLink.h"
HouzRfLink houzLink;

void setup(){
  Serial.begin(115200);
  Serial.println("\n\n::: WeMos Houz Server :::");
  wemosWiFi.connect();

//fs setup
  fsInit();
  wsInit();
  webInit();

//rf setup
  houzLink.init();
  updateTimerInit();
};

void loop(){
  wemosWiFi.update();
  uiUpdate();
  houzCore.timer();
  updateTimer();
  if(houzLink.hasData()){
    handleRecevice(houzLink.update());
  };
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// updateTimer
#define nodeUpdate_pending 0
#define nodeUpdate_waiting 1
#define nodeUpdate_ok      2
#define nodeUpdate_idle    3

struct NodeUpdate{
  u8 nodeId;
  unsigned long timeout;
  u8 status;
  int currNode;
};
int nodeList[] = {living_node, office_node, suite_node};
int nodeListLenght = 2;
NodeUpdate nodeUpdate;
void updateTimerInit(){
  
  nodeUpdate.nodeId = nodeList[0];
  nodeUpdate.currNode = 0;
  nodeUpdate.status = nodeUpdate_pending;
};
void updateTimer(){
  switch (nodeUpdate.status)
  {
  case nodeUpdate_idle:
  case nodeUpdate_pending:
    if(millis()<nodeUpdate.timeout) return;
    return upateTimer_call();
    break;

  case nodeUpdate_ok:
  case nodeUpdate_waiting:
    if(millis()<nodeUpdate.timeout) return;
    //TODO: store node status
    upateTimer_next();
    break;
  
  default:
    break;
  }
};

//TODO: make timers configurable via API
void upateTimer_next(){
  //seek next node
  nodeUpdate.currNode++;
  if(nodeUpdate.currNode>nodeListLenght){
    nodeUpdate.status=nodeUpdate_idle;
    nodeUpdate.currNode=0;
    nodeUpdate.timeout=millis()+600000; //10m: (5*60*1000));
  }else{
    nodeUpdate.status=nodeUpdate_pending;
    nodeUpdate.timeout=millis()+10000;  //10s: (2*1000));
  }
  nodeUpdate.nodeId = nodeList[nodeUpdate.currNode];
}

void upateTimer_call(){
    nodeUpdate.status=nodeUpdate_waiting;
    nodeUpdate.timeout=millis()+1000;
    DevicePkt pkt;
    pkt.id=nodeUpdate.nodeId;
    pkt.node=nodeUpdate.nodeId;
    pkt.payload=0;
    pkt.cmd=10;
    if(houzLink.send(pkt)){
      nodeUpdate.status=nodeUpdate_ok;
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
      Serial.println((int)ws.count());
      break;

    case WS_EVT_DISCONNECT:
      Serial.print("disconnected ");
      Serial.println((int)ws.count());
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
  DynamicJsonDocument jdev(1024);
  DeserializationError error = deserializeJson(jdev, msg);
  if (error) {
    Serial.println("error parsing message");
    return;
  }
  DevicePkt dev;
  dev.node = jdev["node"];
  dev.id = jdev["id"];
  dev.cmd = jdev["cmd"];
  dev.payload = jdev["payload"];

  //deliver packet
  houzLink.send(dev);

  //ack client
  handleServerAct(action_ack);
};

void handleRecevice(Device dev){
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
