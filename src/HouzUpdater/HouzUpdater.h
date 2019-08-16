#pragma once
#include "Arduino.h"
#include "../HouzRfLink/HouzRfLink.h"

class HouzUpdater {
public:
    HouzUpdater(HouzRfLink &houzLink);
    void init();
    void update();
    void setInterval(int _action, int _seconds);

private:
    HouzRfLink* link;
    void timerNext();
    void timerCall();
    int  nextInterval;
    int  idleInterval;
};