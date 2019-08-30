/////////////////////////////
// Houz Office node
//

//Houz
#include "src/HouzCore/HouzCore.h"
#include "src/HouzCore/HouzJson.h"
#include "01_Office.h"
HouzCore houzCore;
OfficeNode office(&houzCore);

//WeMos
#include "src/wemos_WiFi/wemos_WiFi.h"
#include "Arduino.h"

//WebServer
#include <FS.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
AsyncWebServer server(80);
AsyncWebSocket ws("/");

void setup(){
  Serial.begin(115200);
  Serial.println("\n\n:: Houz office node");

  wemosWiFi.connect("houz_office");
  office.setup();
  webInit();


};

void loop(){
  office.update();
  wemosWiFi.update();
};



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WebServer
void webInit(){
  //server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
  server.on("/api",HTTP_GET,onGetStatus);
  server.on("/api/task", HTTP_POST, nullAsyncWebServerRequest, nullFileRequest, onPostTask);
  server.onNotFound(onNotFound);
  server.begin();
  Serial.println("server\tonline");
}

void nullFileRequest(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){};
void nullAsyncWebServerRequest(AsyncWebServerRequest *request){};
void onNotFound(AsyncWebServerRequest *request){request->send(404, "text/plain", "Houz :: Not found");};


void onGetStatus(AsyncWebServerRequest *request){
  String _json = "{";
  _json += "\"node\":\"office\",";
  _json += "\"dev\":[" + JSON.serialize(office.ceilingLight) + "],";
  _json += "\"env\":" + JSON.serialize(office.enviroment);
  _json += "}";

  request->send(200, "application/json", _json);
  _json = String();
}

void onPostTask(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
  try
  {
    houzCore.setTask(JSON.parseDevicePkt((char*)data));
    request->send(200, "application/json", "{\"status\":\"ok\"}");
  }
  catch(const std::exception& e)
  {
    request->send(500, "application/json", "{\"status\":\"error parsing task\"}");
  }
}