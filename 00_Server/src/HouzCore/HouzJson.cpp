#include "HouzJson.h"
#include "devs.h"
#include "Arduino.h"

String HouzJson::serialize(Enviroment env){
    String _env = "{";
    _env += genAttr("temp", env.temp) + ",";
    _env += genAttr("humidity", env.humidity) + ",";
    _env += genAttr("pressure", env.pressure) + ",";
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