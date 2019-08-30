#include "HouzJson.h"
#include "HouzCore.h"
#include "Arduino.h"
#include <ArduinoJson.h>

String HouzJson::serialize(Enviroment env){
    String _env = "{";
    _env += genAttr("temp", env.temp) + ",";
    _env += genAttr("humidity", env.humidity) + ",";
    _env += genAttr("pressure", env.pressure) + ",";
    _env += genAttr("dewPoint", env.dewPoint) + ",";
    _env += genAttr("light", env.light);
    _env += "}";
    return _env;
};
String HouzJson::serialize(Device device){
    String _dev = "{";
    _dev += genAttr("id", device.id) + ",";
    _dev += genAttr("node", device.node) + ",";
    _dev += genAttr("payload", device.payload) + ",";
    _dev += genAttr("type", device.type);
    _dev += "}";
    return _dev;
};

DevicePkt HouzJson::parseDevicePkt(String jsonDevicePkt){
  DevicePkt dev;
  DynamicJsonDocument jdev(1024);
  DeserializationError error = deserializeJson(jdev, jsonDevicePkt);
  if (error) {
    dev.cmd=0;
    Serial.println("HouzJson::parseDevicePkt| error parsing message");
    return dev;
  }
  dev.node = jdev["node"];
  dev.id = jdev["id"];
  dev.cmd = jdev["cmd"];
  dev.payload = jdev["payload"];
  return dev;
};


String HouzJson::genAttr(String _var, float _val){
    String _attr= "\""+ _var + "\":";
    _attr += _val;
    return _attr;
};
String HouzJson::genAttr(String _var, int _val){
    String _attr= "\""+ _var + "\":";
    _attr += _val;
    return _attr;
};
String HouzJson::genAttr(String _var, unsigned long _val){
    String _attr= "\""+ _var + "\":";
    _attr += _val;
    return _attr;
};

HouzJson JSON;