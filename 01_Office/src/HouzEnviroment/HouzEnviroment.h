#pragma once
#include "../HouzCore/devs.h"
#include <BME280I2C.h>
#include <EnvironmentCalculations.h>


class HouzEnviroment{
public:
  HouzEnviroment();
  bool ready;
  bool init();
  Enviroment get();
  
private:
  BME280I2C bme;
  Enviroment current;
  unsigned long voidTS;
  void bmeRead();

};