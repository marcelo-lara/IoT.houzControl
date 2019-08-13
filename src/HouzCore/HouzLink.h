#pragma once
#include "HouzCore.h"
#include "devs.h"

class HouzLink{
    public:
        virtual bool status() = 0;
        virtual void init() = 0;
        virtual bool hasData() = 0;
        virtual DevicePkt update() = 0;
};