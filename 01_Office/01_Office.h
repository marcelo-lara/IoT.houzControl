#pragma once
// Wiring
#define lightSensor A0 // A0 LightSensor
#define wallSwitch  13 // D7 Wall Switch
#define bme280_scl   5 // D1 SCL (bme280)
#define bme280_sda   4 // D2 SDA (bme280)
#define statusLed    0 // D3 Wall StatusLed
#define wifiLed      2 // D4 builtIn led
#define relayOut    12 // D6 relay out
#define irSendPin   14 // D5 infrared signal out

#include "src/HouzCore/HouzButton.h"
#include "src/HouzCore/HouzCore.h"
#include "Arduino.h"

typedef class OfficeNode {
public:
  OfficeNode(HouzCore* _core);
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
  bool setCeilingLightStatus(int _state);
  bool getCeilingLightStatus();
  HouzCore* core;

};