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


//Radio
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "src/HouzCore/devs.h"
#define rf_ce   0 //D3
#define rf_csn 15 //D8
RF24 radio(rf_ce,rf_csn);

//WebServer
#include <FS.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266Wifi.h>
#include <ArduinoJson.h>
AsyncWebServer server(80);
AsyncWebSocket ws("/");

//Houz
#include "src/HouzDevicesCodec/HouzDevicesCodec.h"
HouzDevicesCodec codec;

typedef struct devData {
public:
  int id;
  int node;
  unsigned long data;
  unsigned long update;
};

void setup(){
  Serial.begin(115200);
  Serial.println("\n\n::: WeMos Houz Server :::");
  wemosWiFi.connect();

//fs setup
  fsInit();
  wsInit();
  webInit();

//rf setup
  rfConnect();
    //TODO:retrieve current values from nodes

};

void loop(){
  wemosWiFi.update();
  rfUpdate();
  uiUpdate();
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Radio
bool radioReady;
void rfConnect(){
    radioReady=false;
    radio.begin();
    radio.setPALevel(RF24_PA_HIGH); //RF24_PA_HIGH | RF24_PA_LOW | RF24_PA_MAX
    radio.setDataRate(RF24_1MBPS);
    radio.enableDynamicAck();
    radio.setCRCLength(RF24_CRC_8);
    radio.setChannel(rfChannel);
    radio.setRetries(20, 10);  

    //pipes setup | server node
    radio.openWritingPipe(rf_server_tx);
    radio.openReadingPipe(1, rf_office_tx);
    radio.openReadingPipe(2, rf_suite_tx);
    radio.openReadingPipe(3, rf_living_tx);
    radio.openReadingPipe(4, rf_wemos_tx);

    //debug
    radioReady = (rfChannel == radio.getChannel()); //test if radio is enabled
    radio.printDetails();
}

void rfUpdate(){

	//if radio is not enabled, discard anything
	if (!radioReady) return;
	uint8_t _radioNode;
	if (!radio.available(&_radioNode)) return;

	//get payload
	unsigned long _radioPayLoad;
	while (radio.available()) {
		radio.read(&_radioPayLoad, sizeof(unsigned long));
	}

	//prepare for next packet
	radio.startListening();

  //notify ui
  uiNotify();

	//decode payload
	deviceData device = codec.decode(_radioPayLoad, _radioNode);
  Serial.print("RFrec>\tid: ");
  Serial.println(device.id);
  handleRecevice(device);

}

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

bool rfSend(DevicePkt dev){

	//open write pipe
	uint64_t writeAddress;
  switch (dev.node) {
		case office_node: writeAddress = rf_office_rx; break;
		case suite_node: writeAddress = rf_suite_rx; break;
		case living_node: writeAddress = rf_living_rx; break;
		case wemos_node: writeAddress = rf_wemos_rx; break;
    default:
      Serial.printf("[error] node %i not found\n", dev.node);
      return false;
  };
  u32 msg = rfEncode(dev);

  Serial.print("RFsnd> n");
  Serial.print(dev.node);
  Serial.print(":");
  Serial.print(msg, HEX);

  //rf send
  radio.stopListening();
  radio.openWritingPipe(writeAddress);
  bool result = 0;
  result = radio.write(&msg, sizeof(unsigned long), 0);

  radio.startListening();
  Serial.println(result?" ok":" error");
}

unsigned long rfEncode(DevicePkt dev){
	unsigned long retVal = 0xD;
	retVal = (retVal << 4) + dev.cmd;
	retVal = (retVal << 8) + dev.id;
	retVal = (retVal << 16) + dev.payload;
	return retVal;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FileSystem
void fsInit(){
  if(SPIFFS.begin()){
    Serial.println("SPIFFS loaded");
    server.serveStatic("/", SPIFFS, "/index.html");
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
      handleConnect(client->id());
      break;

    case WS_EVT_DISCONNECT:
      Serial.print("disconnected ");
      Serial.println(client->remoteIP());
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
// HouzServer



String devDataToJson(devData dev){
    String msg;
    msg += "{\"id\":";
    msg += dev.id;

    if(dev.id==external_temp||dev.id==suite_temp||dev.id==suite_humidity||dev.id==external_humidity){
      char buffer[10];
      msg += ",\"fVal\":";
      msg += dtostrf(((float)dev.data/100), -5, 2, buffer);
    //TODO:handle pressure
    }else{
      msg += ",\"iVal\":";
      msg += dev.data;
    };

    msg += "}";
    return msg;
};

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
  rfSend(dev);
};

void handleRecevice(deviceData dev){

  updateDevice(dev);

  //build json
  String msg = "{\"act\":4,\"dev\":";
  devData d;
  d.id = dev.id;
  d.data = dev.payload;
  msg += devDataToJson(d);
  msg += "}";

  //broadcast
  ws.textAll(msg);

}


int nodes[] = {living_node, office_node, suite_node};
devData devices[] = {
//living 
  {0x33,3,0,0}, //mainLight	
  {0x34,3,0,0}, //dicroLight	
  {0x35,3,0,0}, //spotLight	
  {0x36,3,0,0}, //fxLight		
//suite
  {0x21,2,0,0}, //light		    
  {0x22,2,0,0}, //fan			    
  {0x23,2,0,0}, //AC			    
  {0x25,2,0,0}, //enviroment	
  {0x26,2,0,0}, //temp			  
  {0x27,2,0,0}, //humidity  	
  {0x28,2,0,0}, //pressure	
//office	
  {0x11,1,0,0}, //AC			    
  {0x12,1,0,0}, //AC_temp		
  {0x13,1,0,0}, //light		  
  {0x17,1,0,0}, //light		
  {0x1A,1,0,0}, //temp		  
  {0x1B,1,0,0}, //humidity	
  {0x1C,1,0,0}, //pressure	
  {0x1F,1,0,0} //weather	
};

// device status
void updateDevice(deviceData dev){
  for (int i = 0; i < 20; i++)
  {
    if(devices[i].id==dev.id){
      devices[i].data=dev.payload;
      return;
    }
  }
  Serial.println("NOT FOUND!!!!");
}

// update client
void handleConnect(int clientId){
  Serial.print(clientId);
  Serial.println(" connected..");
  
  int devCount = sizeof(devices) / sizeof(devData);
  String msg = "{\"act\":17,\"dev\":[";
  for (int i = 0; i < devCount; i++)
  {
    msg += devDataToJson(devices[i]);
    msg += ",";
  }
  msg = msg.substring(0,msg.length()-1);
  msg += "]}";
  ws.text(clientId, msg);
};
