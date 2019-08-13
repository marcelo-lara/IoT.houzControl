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
};
