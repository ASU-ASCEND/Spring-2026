#ifndef RTC_SENSOR_H
#define RTC_SENSOR_H

#include "Sensor.h"

#include "RTClib.h"

/**
 * @brief Implementation of a Sensor for the real time clock
 *
 */
class RTCSensor : public Sensor {
 private:
  RTC_PCF8523 rtc;
 public:
  RTCSensor();
  bool verify() override;
  void readToSysVar() override;
};

#endif