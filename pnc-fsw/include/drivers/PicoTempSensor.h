#ifndef TEMP_SENSOR_H
#define TEMP_SENSOR_H

#include "Sensor.h"

/**
 * @brief Implementation of a Sensor for the onboard temperature sensor
 *
 */
class PicoTempSensor : public Sensor {
 private:
 public:
  PicoTempSensor();
  bool verify() override;
  void readToSysVar() override;
};

#endif