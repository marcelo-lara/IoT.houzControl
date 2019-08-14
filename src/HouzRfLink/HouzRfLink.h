#pragma once
#include "../HouzCore/HouzCore.h"
#include "../HouzCore/HouzLink.h"

class HouzRfLink : public HouzLink {
private:
public:
    HouzRfLink();
    ~HouzRfLink();

    bool status();
    void init();
    bool hasData();
    DevicePkt update();
    bool send(DevicePkt dev);

private:
    bool radioReady;
    unsigned long rfEncode(DevicePkt dev);
    DevicePkt rfDecode(unsigned long payload, int nodeId);
    void sendNext();
};
