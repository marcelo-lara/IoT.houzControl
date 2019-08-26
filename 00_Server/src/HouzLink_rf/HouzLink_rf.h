#pragma once
#include "../HouzCore/HouzCore.h"
#include "../HouzCore/HouzLink.h"

class HouzLink_rf : public HouzLink {
public:
    HouzLink_rf();
    ~HouzLink_rf();

    bool status();
    void init();
    bool hasData();
    DevicePkt getData();
    bool send(DevicePkt dev);

private:
    bool radioReady;
    unsigned long rfEncode(DevicePkt dev);
    DevicePkt rfDecode(unsigned long payload, int nodeId);
    void sendNext();
    DevicePkt getActNotification(int action);
    DevicePkt getDevice(int nodeId, int deviceId, u32 payload);

};
