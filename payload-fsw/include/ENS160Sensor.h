#ifndef ENS160_SENSOR_H
#define ENS160_SENSOR_H

#include <Wire.h>

#include "PayloadConfig.h"
#include "Sensor.h"
#include "SparkFun_ENS160.h"

/**
 * @brief Implementation of a Sensor for the ENS160
 *
 */
class ENS160Sensor : public Sensor {
 private:
  SparkFun_ENS160 ens;
  TwoWire* i2c_bus;
  void setCompensations();
  uint8_t i2c_addr; 

 public:
  ENS160Sensor(TwoWire* i2c_bus = &Wire, uint8_t i2c_addr = ENS160_ADDRESS_HIGH);
  ENS160Sensor(unsigned long minium_period, TwoWire* i2c_bus = &Wire, uint8_t i2c_addr = ENS160_ADDRESS_HIGH);
  bool verify() override;
  String readData() override;
  void readDataPacket(uint8_t*& packet) override;
  String decodeToCSV(uint8_t*& packet) override;
};

#endif