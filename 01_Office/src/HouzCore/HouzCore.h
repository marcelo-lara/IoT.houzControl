#pragma once
#include "Arduino.h"
#include "ActionsEnm.h"
#include <EEPROM.h>
#include "devs.h"

// commands
#define CMD_QUERY			0xA
#define CMD_VALUE			0xB
#define CMD_SET				0xC
#define CMD_EVENT			0xD
#define CMD_STATUS			0xE

// scenes
#define scene_Goodbye	0xCF0
#define scene_Sleep		0xCF1
#define scene_Hello		0xCF2

// sw button
#define swSingleClick	0xA01
#define swDoubleClick	0xA02
#define swLongPress		0xA10
#define swClickLong		0xA11

class HouzCore{
  public:
    HouzCore();
    void timer();

    void updateDevice(Device dev);
    Device getDevice(u8 deviceId);
    void showDevice(DevicePkt devPkt, Stream* out);

    //tasks
    bool hasTask();
    DevicePkt getTask();
    bool setTask(DevicePkt _task);

    //json
    String json_getDeviceList();
    String json_getDevice(u8 deviceId);

    //enviroment helper
    void enviromentPush(float temp);



  private:
    String json_getDevice(Device dev);
    void persist(Device dev, int devIndex);
    void readDevices();
    void persistReset();
};