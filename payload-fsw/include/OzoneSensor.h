#ifndef OZONE_SENSOR_H
#define OZONE_SENSOR_H

#include "DFRobot_OzoneSensor.h"
#include "PayloadConfig.h"
#include "Sensor.h"

/**
 * @brief Implementation of a Sensor for the onboard temperature sensor
 *
 */
class OzoneSensor : public Sensor {
 private:
  DFRobot_OzoneSensor ozone;
  TwoWire* i2c_bus;

 public:
  OzoneSensor(TwoWire* i2c_bus = &Wire);
  OzoneSensor(unsigned long minium_period, TwoWire* i2c_bus = &Wire);

  bool verify() override;
  String readData() override;
  void readDataPacket(uint8_t*& packet) override;
  String decodeToCSV(uint8_t*& packet) override;
};

#endif  // OZONE_SENSOR_H