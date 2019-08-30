#include "HouzCore.h"
#include "Model/devs.h"
#include "ActionsEnm.h"
#include "Arduino.h"
#include <iostream>
#include <iomanip>

#include <QueueArray.h>
QueueArray <DevicePkt> taskQueue;
template<class T> inline Print &operator <<(Print &obj, T arg) { obj.print(arg); return obj; }

//////////////////////
// devices
int __nodes[] = {
    living_node, 
    office_node, 
    suite_node
};
int __devices_count = 0;

Device devices[]={
        //living 
        {living_node,         living_node,0,deviceType_node}, 
        {living_mainLight,    living_node,0,0}, 
        {living_dicroLight,   living_node,0,0}, 
        {living_spotLight,    living_node,0,0}, 
        {living_fxLight,      living_node,0,0}, 
        //suite
        {suite_node,          suite_node,0,deviceType_node}, 
        {suite_light,         suite_node,0,0}, 	    
        {suite_fan,           suite_node,0,0}, 	    
        {suite_AC,            suite_node,0,0}, 	    
        {suite_enviroment,    suite_node,0,0}, 	
        {suite_temp,          suite_node,0,deviceType_float}, 	  
        {suite_humidity,      suite_node,0,deviceType_float}, 	
        {suite_pressure,      suite_node,0,deviceType_pressure}, 
        //office
        {office_node,         office_node,0,deviceType_node}, 
        {office_AC,           office_node,0,0},
        {office_AC_temp,      office_node,0,0},
        {office_light,        office_node,0,0},
        {external_light,      office_node,0,0},
        {external_temp,       office_node,0,deviceType_float},
        {external_humidity,   office_node,0,deviceType_float},
        {external_pressure,   office_node,0,deviceType_pressure},
        {external_weather,    office_node,0,0} 
    };


HouzCore::HouzCore(){
    Serial.println("-HouzCore init-");
    __devices_count = sizeof(devices) / sizeof(Device);
    EEPROM.begin(512);
    readDevices();
}

void HouzCore::timer(){
  
}

////////////////////////////////////////////////////////////////////////////////////////////////
// EEPROM persistence
void HouzCore::persist(Device dev, int devIndex){
  int addr = devIndex*sizeof(Device);
  EEPROM.put(addr, dev);
  EEPROM.commit();
};
void HouzCore::readDevices(){
  //retrieve eeprom values
  for (int i = 0; i < __devices_count; i++)
  {
    Device _mem;
    int addr = i*sizeof(Device);
    bool commitNeeded = false;
    EEPROM.get(addr,_mem);
    if(_mem.id==devices[i].id){
      devices[i].payload=_mem.payload;
    }else{
      EEPROM.put(addr, devices[i]);
      commitNeeded = true;
    }
    if(commitNeeded) EEPROM.commit();
  }
};

Device HouzCore::getDevice(u8 deviceId){
  for (int i = 0; i < __devices_count; i++)
  {
    if(devices[i].id==deviceId){
      return devices[i];
    }
  }
  return {-1,0,0,0};
};

void HouzCore::updateDevice(Device dev){
  for (int i = 0; i < __devices_count; i++)
  {
    if(devices[i].id==dev.id){
      devices[i].payload=dev.payload;
      persist(dev, i);
      return;
    }
  }
}
void HouzCore::updateDevice(int devId, unsigned long payload){
  Device dd;
  dd.id=devId;
  dd.payload=payload;
  updateDevice(dd);
}


////////////////////////////////////////////////////////////////////////////////////////////////
// JSON responses
String HouzCore::json_getDeviceList(){
  String msg = "{\"act\":17,\"dev\":[";
  for (int i = 0; i < __devices_count; i++)
  {
    msg += json_getDevice(devices[i]);
    msg += ",";
  }
  msg = msg.substring(0,msg.length()-1);
  msg += "]}";
  return msg;
};

String HouzCore::json_getDevice(u8 deviceId){
  Device dev = getDevice(deviceId);
  if(dev.id==-1) return "";
  return json_getDevice(dev);
};

String HouzCore::json_getDevice(Device dev){
    String msg;
    msg += "{\"id\":";
    msg += dev.id;
    msg += ",\"type\":";
    msg += dev.type;
    if(dev.type==deviceType_float){
      char buffer[10];
      msg += ",\"fVal\":";
      msg += dtostrf(((float)dev.payload/100), -5, 2, buffer);
    }else if(dev.type==deviceType_pressure){
      char buffer[10];
      msg += ",\"fVal\":";
      msg += dtostrf(((float)dev.payload/100)+900, -5, 2, buffer);
    }else{
      msg += ",\"iVal\":";
      msg += dev.payload;
    };
    msg += "}";
    return msg;
};

void HouzCore::showDevice(DevicePkt devPkt, Stream* out){
  out->print("n");
  out->print(devPkt.node);
  out->print(".");
  out->print(devPkt.cmd, HEX);
  out->print(devPkt.id, HEX);
  out->print(devPkt.payload, HEX);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//tasks
bool HouzCore::hasTask(){
  return !taskQueue.isEmpty();
};
DevicePkt HouzCore::getTask(){
  return taskQueue.dequeue();
};
bool HouzCore::setTask(DevicePkt _task){
  taskQueue.enqueue(_task);
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// weather

