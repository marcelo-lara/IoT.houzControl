#include "HouzRfLink.h"
#include "Arduino.h"

HouzRfLink::HouzRfLink(){
    Serial.print("HouzRfLink::HouzRfLink()");
}

HouzRfLink::~HouzRfLink(){
    Serial.print("HouzRfLink::~HouzRfLink()");
}

void HouzRfLink::init(){
    Serial.print("HouzLink::init()");
}

bool HouzRfLink::hasData(){
    Serial.print("HouzLink::hasData()");
}

bool HouzRfLink::status(){
    Serial.print("HouzLink::status()");
}

DevicePkt HouzRfLink::update(){
    Serial.print("HouzLink::update()");
}

