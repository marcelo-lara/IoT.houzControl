#include "HouzEnviroment.h"
#include "Arduino.h"

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

HouzEnviroment::HouzEnviroment(){};

bool HouzEnviroment::init(){
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
  if(bmeFound) bmeRead();
};

Enviroment HouzEnviroment::get(){
   bmeRead();
   return current;
};

void  HouzEnviroment::bmeRead(){
  //read values
   float temp(NAN), hum(NAN), pres(NAN);
   BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
   BME280::PresUnit presUnit(BME280::PresUnit_hPa);
   bme.read(pres, temp, hum, tempUnit, presUnit);
   
  //store read values
   current.dewPoint = EnvironmentCalculations::DewPoint(temp, hum, EnvironmentCalculations::TempUnit_Celsius);
   current.temp=temp;
   current.humidity=hum;
   current.pressure=pres;

   //debug
   Serial.print("env\t");
   Serial.print(temp);
   Serial.println("º");

};