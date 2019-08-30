#pragma once
#include "Arduino.h"
#include "HouzCore.h"

typedef class HouzJson{
public:
    String serialize(Enviroment env);
    String serialize(Device device);
    DevicePkt parseDevicePkt(String jsonDevicePkt);
private:
    String genAttr(String _var, float _val);
    String genAttr(String _var, int _val);
    String genAttr(String _var, unsigned long _val);
};
extern HouzJson JSON;