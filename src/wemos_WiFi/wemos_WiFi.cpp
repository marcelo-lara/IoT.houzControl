#include "Arduino.h"
#include <darkblue.h>
#include "wemos_WiFi.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

void wemos_WiFi::connect() {
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
  if (MDNS.begin(STAHOST)) 
    MDNS.addService("http", "tcp", 80);

}

void wemos_WiFi::update() {
    MDNS.update();
}

wemos_WiFi wemosWiFi;