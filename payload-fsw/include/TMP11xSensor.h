#ifndef TMP117_SENSOR_H
#define TMP117_SENSOR_H

#include <SparkFun_TMP117.h>
#include <Wire.h>

#include "PayloadConfig.h"
#include "Sensor.h"

/**
 * @brief Implementation of a Sensor for the TMP117
 *
 */
class TMP11xSensor : public Sensor {
 private:
  TMP117 tmp;
  float tempC;
  TwoWire* i2c_bus;

 public:
  TMP11xSensor(TwoWire* i2c_bus = &Wire);
  TMP11xSensor(unsigned long minium_period, TwoWire* i2c_bus = &Wire);
  bool verify();
  String readData();

  // Function to integrate sensor data into a packet buffer.
  void readDataPacket(uint8_t*& packet);

  // Function to decode sensor data from a packet and return a CSV string
  String decodeToCSV(uint8_t*& packet);

  float getTempC();
};

#endif