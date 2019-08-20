#define inSwitch_debounce 20
#define inSwitch_click 300
#define inSwitch_longPress 1000
#define inSwitch_timeout 400

#define btn_down 0
#define btn_up 1

#include "HouzButton.h"
#include "HouzCore.h"
#include "Arduino.h"

HouzCore* core;
Device device;
uint8_t switchButton;
int _lastStatus;
int _currStatus;

HouzButton::HouzButton(u8 _deviceId, uint8_t _switchButton){
    device.id=_deviceId;
    switchButton=_switchButton;
    pinMode(switchButton, INPUT_PULLUP);
	_lastStatus=digitalRead(switchButton);
};

void HouzButton::setup(HouzCore* _core){
  core=_core;
};

unsigned long _sw_timerStart;
unsigned long _sw_nextClickTimeout;
unsigned long _sw_longPressTimeout;
unsigned long _sw_minNextClick;
int _sw_clickCount = 0;

void HouzButton::update(){
	_currStatus=digitalRead(switchButton);
	
	//are we counting clicks?
	if(_sw_clickCount>0){

		//debounce ignore
		if(_sw_minNextClick<inSwitch_debounce){
			Serial.println("debounce> ignoring");
      return;
		}

    //button released.. click timeout?
    if(_currStatus==btn_up && _sw_nextClickTimeout<millis()){
      //no more wait, send click event
      core->setTask(getPkt(0xA0+_sw_clickCount));

      //reset counters
      _sw_clickCount=0;
      _lastStatus=_currStatus;
    }

    //it's longpress timeout?
    if(_sw_longPressTimeout<millis()){
      //send longpress event
      core->setTask(getPkt(0xB0+_sw_clickCount));

      //reset counters
      _sw_clickCount=0;
      _lastStatus=_currStatus;
    }
    
	}

	//we're not counting clicks, something changed?
	if(_lastStatus==_currStatus) return;
	_lastStatus=_currStatus;

	//button is pressed
	if(_currStatus==btn_down){
		_sw_clickCount++;
		_sw_timerStart=millis();
		_sw_longPressTimeout=_sw_timerStart+inSwitch_longPress;
		_sw_nextClickTimeout=millis()+inSwitch_timeout;
		_sw_minNextClick=millis()+inSwitch_debounce;
	}

	//button is released
	else{

    //maybe released after longpress, ignoring
    if(_sw_clickCount==0) return;

    //count click
		unsigned long elapsed=millis()-_sw_timerStart;
		_sw_nextClickTimeout=millis()+inSwitch_timeout;
	}

}

DevicePkt HouzButton::getPkt(unsigned long action){
    DevicePkt dev;
    dev.id=device.id;
    dev.node = device.node;
    dev.cmd = CMD_SET;
    dev.payload = action;
    return dev;
}