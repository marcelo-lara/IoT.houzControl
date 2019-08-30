#pragma once

#include "01_Office.h"
#include "src/HouzCore/HouzCore.h"
#include "src/HouzCore/HouzButton.h"
#include "Arduino.h"

//Infrared
#include <IRremoteESP8266.h>
#include <IRsend.h>
IRsend irsend(irSendPin);

//Enviroment
#include "src/HouzEnviroment/HouzEnviroment.h"
HouzEnviroment envSensor;

//PushButton
#include "src/HouzCore/HouzButton.h"
HouzButton button(office_switch, wallSwitch);


OfficeNode::OfficeNode(HouzCore* _core){
  pinMode(statusLed,  OUTPUT);
  pinMode(relayOut,   OUTPUT);
  setCeilingLightStatus(1);
  digitalWrite(statusLed, HIGH);

  button.setup(_core);
  core = _core;

  //set devices
  ceilingLight.id = office_light;
  ceilingLight.node = office_node;

}

void OfficeNode::setup(){

  envSensor.init();
  irsend.begin();
  analogWrite(statusLed, 200);
  setCeilingLightStatus(0);
  enviroment = envSensor.get();

};

void OfficeNode::update(){
  button.update();
  if(!core->hasTask()) return;
  DevicePkt dev=core->getTask();
  switch (dev.id){

  case office_switch: 
    handle_WallSwitch(dev); 
    break;
  
  case office_AC:
    setAC();
    break;

  default:
    core->showDevice(dev, &Serial);
    Serial.println("???");
    break;
  }

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enviroment bme280


Enviroment OfficeNode::getEnviroment(){
  enviroment = envSensor.get();
  return enviroment;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wall Switch
void OfficeNode::handle_WallSwitch(DevicePkt dev){
  switch (dev.cmd){
  
  case CMD_SET:
    switch (dev.payload){
      
      //single click
      case action_swClick:
        setCeilingLightStatus(-1);//toggle
        break;

      //double click
      case action_swDblClick:
        setCeilingLightStatus(0);
        break;

      //long press
      case action_swLongPress:
        //set sleep scene
        break;

      default:
        break;
      }
    break;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Ceiling Light

//set ceilingLight relay status
bool OfficeNode::setCeilingLightStatus(int _state){
  if(_state==-1) _state=!(getCeilingLightStatus());//toggle
  if(_state>1) _state=1;
  digitalWrite(relayOut, !_state);

  ceilingLight.payload=_state;
}
//get ceilingLight relay status
bool OfficeNode::getCeilingLightStatus(){
  return digitalRead(relayOut)==0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Air Conditioner
void OfficeNode::setAC(){
  //TODO: implement https://github.com/crankyoldgit/IRremoteESP8266/blob/master/examples/LGACSend/LGACSend.ino; or better, create AC delegate
	Serial.println("setAC");
  irsend.sendLG(0x88C0051, 28); //turn off 
}
