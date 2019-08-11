#pragma once
#include "Arduino.h"
#include <EEPROM.h>
#include "devs.h"

class HouzCore{
  public:
    HouzCore();
    void timer();

    void updateDevice(Device dev);
    Device getDevice(u8 deviceId);

    //json
    String json_getDeviceList();
    String json_getDevice(u8 deviceId);

  private:
    String json_getDevice(Device dev);
    void persist(Device dev, int devIndex);
    void readDevices();
    void persistReset();
};