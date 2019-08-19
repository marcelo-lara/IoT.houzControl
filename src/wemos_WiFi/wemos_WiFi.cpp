#include "Arduino.h"
#include <darkblue.h>
#include "wemos_WiFi.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

void wemos_WiFi::connect() {
  connect(STAHOST);
};
void wemos_WiFi::connect(const char* _nodeName) {
  Serial.print("connecting");

  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.print(".");
  }
  Serial.print("\rconnected to ");
  Serial.print(STASSID);
  Serial.print(" @");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_BUILTIN, HIGH);  

  // mDNS service
  if (MDNS.begin(_nodeName)) 
    MDNS.addService("http", "tcp", 80);

  // OTA update
  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

void wemos_WiFi::update() {
    MDNS.update();
    ArduinoOTA.handle();
}

wemos_WiFi wemosWiFi;