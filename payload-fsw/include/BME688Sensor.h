#ifndef BME688_SENSOR_H
#define BME688_SENSOR_H

#include <Wire.h>

#include "Adafruit_BME680.h"
#include "Sensor.h"

/**
 * @brief Implementation of a Sensor for the BME688
 *
 */
class BME688Sensor : public Sensor {
 private:
  Adafruit_BME680 bme;
  TwoWire* i2c_bus;
  uint8_t i2c_addr;

 public:
  BME688Sensor(TwoWire* i2c_bus = &Wire);
  BME688Sensor(unsigned long minimum_period, TwoWire* i2c_bus = &Wire);

  bool verify() override;
  String readData() override;
  void readDataPacket(uint8_t*& packet) override;
  String decodeToCSV(uint8_t*& packet) override;
};

#endif  // BME688_SENSOR_H
