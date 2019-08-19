#include "HouzCore.h"

typedef class HouzButton{
public:
  HouzButton(u8 _deviceId, uint8_t _switchButton, HouzCore* _core);
  void update();

private:
  DevicePkt getPkt(unsigned long action);
  byte inSwitch;
  bool inSw_lastStatus;
  unsigned long inSw_lastMs;
  unsigned long inSw_downMs;
  int inSwitchCount;

};