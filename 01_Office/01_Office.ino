/////////////////////////////
// Houz Office node
//
// Wiring
#define lightSensor A0 // A0 LightSensor
#define wallSwitch  14 // D0 Wall Switch
#define bme280_scl   5 // D1 SCL (bme280)
#define bme280_sda   4 // D2 SDA (bme280)
#define statusLed    0 // D3 Wall StatusLed
#define wifiLed      2 // D4 builtIn led
#define relayOut    12 // D5 relay out

#include "01_Office.h"
#include "src/wemos_WiFi/wemos_WiFi.h"
#include "Arduino.h"

#include <BME280I2C.h>
#include <Wire.h>
BME280I2C bme;

//Houz
#include "src/HouzCore/devs.h"
#include "src/HouzCore/HouzCore.h"
HouzCore houzCore;

//PushButton
#include "src/HouzCore/HouzButton.h"
HouzButton button(office_switch, wallSwitch, &houzCore);

void setup(){
  //set pins
  // pinMode(wallSwitch, INPUT_PULLUP);  //D0 Wall Switch
  pinMode(statusLed,  OUTPUT);        //D3 Wall StatusLed
  pinMode(relayOut,   OUTPUT);        //D4 builtIn led
  setCeilingLight(1);
  digitalWrite(statusLed, HIGH);

  Serial.begin(115200);
  Serial.println("\n\n:: Houz office node");
  wemosWiFi.connect("houz_office");

  enviromentSetup();

  analogWrite(statusLed, 200);
  setCeilingLight(0);

};

int lastStat = 0;

void loop(){
  wemosWiFi.update();
  button.update();
  if(houzCore.hasTask())
    handleTask(houzCore.getTask());
};

void handleTask(DevicePkt dev){
  switch (dev.id){
  case office_switch: handleWallSwitch(dev); break;
  
  default:
    houzCore.showDevice(dev, &Serial);
    Serial.println("???");
    break;
  }

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Enviroment bme280
void enviromentSetup(){
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

Enviroment getEnviroment(){
  //read values
   float temp(NAN), hum(NAN), pres(NAN);
   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_hPa);
   bme.read(pres, temp, hum, tempUnit, presUnit);

  //store read values
   Enviroment env;
   env.temp=temp;
   env.humidity=hum;
   env.pressure=pres;
   return env;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Wall Switch
void handleWallSwitch(DevicePkt dev){
  switch (dev.cmd){
  
  case CMD_SET:
    switch (dev.payload){
      
      //single click
      case action_swClick:
        setCeilingLight(-1);//toggle
        break;

      //double click
      case action_swDblClick:
        setCeilingLight(0);
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
bool setCeilingLight(int _state){
  if(_state==-1) _state=!(getCeilingLight());//toggle
  if(_state>1) _state=1;
  digitalWrite(relayOut, !_state);
}
bool getCeilingLight(){
  return digitalRead(relayOut)==0;
}