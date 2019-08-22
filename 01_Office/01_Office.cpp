#pragma once

#include "01_Office.h"
#include "src/HouzCore/devs.h"
#include "src/HouzCore/HouzCore.h"
#include "src/HouzCore/HouzButton.h"
#include "Arduino.h"

//Infrared
#include <IRremoteESP8266.h>
#include <IRsend.h>
IRsend irsend(irSendPin);

//Enviroment
#include <BME280I2C.h>
#include <EnvironmentCalculations.h>
#include <Wire.h>
BME280I2C::Settings settings(
   BME280::OSR_X1,
   BME280::OSR_X1,
   BME280::OSR_X1,
   BME280::Mode_Forced,
   BME280::StandbyTime_1000ms,
   BME280::Filter_16,
   BME280::SpiEnable_False,
   BME280I2C::I2CAddr_0x76
);
BME280I2C bme(settings);

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

  enviromentSetup();
  irsend.begin();
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
  Serial.print("bme280\t");
  while(!bmeFound && retry<10)
  {
    bmeFound=bme.begin();
    if(bmeFound) continue;
    Serial.print(".");
    delay(500);
    retry++;
  }
  Serial.println(bmeFound?"online":" offline");
  if(bmeFound) getEnviroment();
};

Enviroment OfficeNode::getEnviroment(){
  //read values
   float temp(NAN), hum(NAN), pres(NAN);
   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_hPa);
   bme.read(pres, temp, hum, tempUnit, presUnit);
   
  //store read values
   enviroment.dewPoint = EnvironmentCalculations::DewPoint(temp, hum, EnvironmentCalculations::TempUnit_Celsius);
   enviroment.temp=temp;
   enviroment.humidity=hum;
   enviroment.pressure=pres;
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
bool OfficeNode::setCeilingLightStatus(int _state){
  if(_state==-1) _state=!(getCeilingLightStatus());//toggle
  if(_state>1) _state=1;
  digitalWrite(relayOut, !_state);

  ceilingLight.payload=_state;
}
bool OfficeNode::getCeilingLightStatus(){
  return digitalRead(relayOut)==0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Air Conditioner
void OfficeNode::setAC(){
  //TODO: implement https://github.com/crankyoldgit/IRremoteESP8266/blob/master/examples/LGACSend/LGACSend.ino; or better, create AC delegate
	irsend.sendLG(0x88C0051, 28); //turn off 
}
