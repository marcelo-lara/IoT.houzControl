#pragma once
// Wiring
#define lightSensor A0 // A0 LightSensor

#define wallSwitch  14 // D5 Wall Switch
#define statusLed   12 // D6 Wall StatusLed

#define relayOut    13 // D7 relay out

#define irSendPin    0 // D3 infrared signal out

#define bme280_scl   5 // D1 SCL (bme280)
#define bme280_sda   4 // D2 SDA (bme280)
#define wifiLed      2 // D4 builtIn led

#include "src/HouzCore/Button/Button.h"
#include "src/HouzCore/HouzCore.h"
#include "Arduino.h"

typedef class OfficeNode {
public:
  OfficeNode(HouzCore &_core);
  void handle_WallSwitch(DevicePkt dev);
  void setup();
  void update();
  Enviroment getEnviroment();
  void enviromentSetup();
  Device getCeilingLight();

  //devices
  Device ceilingLight;
  Enviroment enviroment;
  void setAC(); //TODO: handle air conditioner fx

private:
  void setCeilingLightStatus(int _state);
  bool getCeilingLightStatus();
  HouzCore* core;

};
