/////////////////////////////
// Houz Office node
//

//Houz
#include "src/HouzCore/HouzCore.h"
#include "src/HouzCore/devs.h"
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
  server.onNotFound(onNotFound);
  server.begin();
}

void onNotFound(AsyncWebServerRequest *request){
    request->send(404, "text/plain", "Houz :: Not found");
}

void onGetStatus(AsyncWebServerRequest *request){
  String json = "{";
  json += "\"node\":\"office\",";
  json += "\"light\":"+ (String)(office.getCeilingLight()?"1":"0") + ",";
  json += "\"env\":";

  
  json += "}";

  request->send(200, "application/json", json);
  json = String();
}

