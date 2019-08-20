#pragma once
#include "Arduino.h"
#include "devs.h"

typedef class HouzJson{
    String getEnviroment(Enviroment env);
    String getDevice(Device device);
};