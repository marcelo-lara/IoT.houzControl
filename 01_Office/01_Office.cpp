#pragma once

#include "01_Office.h"
#include "src/HouzCore/devs.h"
#include "src/HouzCore/HouzCore.h"
#include "src/HouzCore/HouzButton.h"
#include "Arduino.h"

//Enviroment
#include <BME280I2C.h>
#include <Wire.h>
BME280I2C bme;

//PushButton
#include "src/HouzCore/HouzButton.h"
HouzButton button(office_switch, wallSwitch);


OfficeNode::OfficeNode(HouzCore* _core){
  pinMode(statusLed,  OUTPUT);        //D3 Wall StatusLed
  pinMode(relayOut,   OUTPUT);        //D4 builtIn led
  setCeilingLightStatus(1);
  digitalWrite(statusLed, HIGH);

  button.setup(_core);
  core = _core;

  //set devices
  ceilingLight.id = office_light;
  ceilingLight.node = office_node;

}

void OfficeNode::setup(){

  enviromentSetup();

  analogWrite(statusLed, 200);
  setCeilingLightStatus(0);

};
void OfficeNode::update(){
  button.update();
  if(!core->hasTask()) return;
  DevicePkt dev=core->getTask();
  switch (dev.id){

  case office_switch: 
    handle_WallSwitch(dev); 
    break;
  
  default:
    core->showDevice(dev, &Serial);
    Serial.println("???");
    break;
  }

};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enviroment bme280
void OfficeNode::enviromentSetup(){
  Wire.begin();
  int retry =0;
  bool bmeFound = false;
  Serial.print("bme280\t\t");
  while(!bmeFound && retry<10)
  {
    bmeFound=bme.begin();
    if(bmeFound) continue;
    Serial.print(".");
    delay(500);
    retry++;
  }
  Serial.println(bmeFound?" online":" offline");
};

Enviroment OfficeNode::getEnviroment(){
  //read values
   float temp(NAN), hum(NAN), pres(NAN);
   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_hPa);
   bme.read(pres, temp, hum, tempUnit, presUnit);

  //store read values
   enviroment.temp=temp;
   enviroment.humidity=hum;
   enviroment.pressure=pres;
   return enviroment;
}

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
bool OfficeNode::setCeilingLightStatus(int _state){
  if(_state==-1) _state=!(getCeilingLightStatus());//toggle
  if(_state>1) _state=1;
  digitalWrite(relayOut, !_state);
  core->updateDevice(office_light, _state);
}
bool OfficeNode::getCeilingLightStatus(){
  return digitalRead(relayOut)==0;
}
