#pragma once
#include "Arduino.h"
#include "../HouzRfLink/HouzRfLink.h"
#include "../HouzCore/HouzCore.h"

class HouzUpdater {
public:
    HouzUpdater(HouzRfLink &houzLink, HouzCore &houzCore);
    void init();
    void update();
    void setInterval(int _action, int _seconds);
    void pauseUpdate();

private:
    HouzRfLink* link;
    HouzCore* core;
    void timerNext();
    void timerCall();
    int  nextInterval;
    int  idleInterval;
};