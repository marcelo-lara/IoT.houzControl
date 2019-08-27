#pragma once
#include "HouzCore.h"
#include "devs.h"

class HouzLink{
    public:
        virtual bool status() = 0;
        virtual void init() = 0;
        virtual bool hasData() = 0;
        virtual DevicePkt getData() = 0;
        virtual bool send(DevicePkt devicePkt) = 0;
};