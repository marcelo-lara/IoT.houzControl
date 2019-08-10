//TODO: implement
#include "HouzCore.h"
#include "devs.h"
#include "Arduino.h"

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
        {living_mainLight,    living_node,0,0}, 
        {living_dicroLight,   living_node,0,0}, 
        {living_spotLight,    living_node,0,0}, 
        {living_fxLight,      living_node,0,0}, 
        //suite
        {suite_light,         suite_node,0,0}, 	    
        {suite_fan,           suite_node,0,0}, 	    
        {suite_AC,            suite_node,0,0}, 	    
        {suite_enviroment,    suite_node,0,0}, 	
        {suite_temp,          suite_node,0,deviceType_float}, 	  
        {suite_humidity,      suite_node,0,deviceType_float}, 	
        {suite_pressure,      suite_node,0,deviceType_pressure}, 
        //office	
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
    mem_read();

    __devices_count = sizeof(devices) / sizeof(Device);
    EEPROM.write(0, __devices_count);
}

void HouzCore::mem_write(){};
void HouzCore::mem_read(){
    int aa;
    EEPROM.get(0, aa);
    Serial.println(aa);
};


Device* HouzCore::getDevices(){
    return devices;
}

void HouzCore::updateDevice(Device dev){
  for (int i = 0; i < __devices_count; i++)
  {
    if(devices[i].id==dev.id){
      devices[i].payload=dev.payload;
      return;
    }
  }
}

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
  return "";
};

String HouzCore::json_getDevice(Device dev){
    String msg;
    msg += "{\"id\":";
    msg += dev.id;


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
