#pragma once
#include "Arduino.h"
#include <EEPROM.h>
#include "devs.h"

class HouzCore{
  public:
    HouzCore();
    Device* getDevices();
    void updateDevice(Device dev);

    //json
    String json_getDeviceList();
    String json_getDevice(u8 deviceId);
    String json_getDevice(Device dev);

  private:
    void mem_write();
    void mem_read();
};