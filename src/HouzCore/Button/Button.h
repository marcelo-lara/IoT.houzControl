#pragma once
#include "../HouzCore.h"

typedef class Button{
public:
  Button(u8 _deviceId, uint8_t _switchButton);
  void setup(HouzCore* _core);
  void update();

private:
  DevicePkt getPkt(unsigned long action);
  byte inSwitch;
  bool inSw_lastStatus;
  unsigned long inSw_lastMs;
  unsigned long inSw_downMs;
  int inSwitchCount;

};