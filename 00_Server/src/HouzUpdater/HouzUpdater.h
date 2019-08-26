#pragma once
#include "Arduino.h"
#include "../HouzLink_rf/HouzLink_rf.h"
#include "../HouzCore/HouzCore.h"

class HouzUpdater {
public:
    HouzUpdater(HouzLink_rf &houzLink, HouzCore &houzCore);
    void init();
    void update();
    void setInterval(int _action, int _seconds);
    void pauseUpdate();

private:
    HouzLink_rf* link;
    HouzCore* core;
    void timerNext();
    void timerCall();
    int  nextInterval;
    int  idleInterval;
};