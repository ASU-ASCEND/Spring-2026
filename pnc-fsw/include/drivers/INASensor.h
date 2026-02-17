#ifndef INA_SENSOR_H
#define INA_SENSOR_H

#include <Arduino.h>
#include <string.h>

#include "Adafruit_INA260.h"
#include "Sensor.h"
#include "Wire.h"

typedef struct INASensorData
{
  float INACurrent;
  float INABusVoltage;
  float INAPower;

} INASensorData;


class INASensor : public Sensor {
 private:
  Adafruit_INA260 ina;

 public:
  INASensor();
  bool verify() override;
  void readToSysVar() override;
};

#endif