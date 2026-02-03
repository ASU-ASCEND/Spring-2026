#include "drivers/RTCSensor.h"

#include "SysHead.h"

RTCSensor::RTCSensor() : Sensor("RTC") {}

bool RTCSensor::verify(){
  if (this->rtc.begin() == false) return false;
  //  only include to reset
  this->rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  this->rtc.start();
  return true;
}

void RTCSensor::readToSysVar(){
  DateTime now = rtc.now();

  log_task("Seconds:" + String(now.second())); 

  log_task("Read time:" + String(now.unixtime())); 
  sysvar_set_rtc_time(now.unixtime()); 
}