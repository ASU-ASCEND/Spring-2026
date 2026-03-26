#ifndef BMP384_SENSOR_H
#define BMP384_SENSOR_H

#include <Wire.h>

#include "Adafruit_BMP3XX.h"
#include "Sensor.h"

#define BMP390_DEFAULT_I2C_ADDR 0x77
#define BMP390_ALT_I2C_ADDR 0x76

/**
 * @brief Implementation of a Sensor for BMP384 Pressure and Temperature sensor
 *
 */
class BMP390Sensor : public Sensor {
 private:
  Adafruit_BMP3XX bmp;
  TwoWire* i2c_bus;
  uint8_t i2c_addr; 

 public:
  BMP390Sensor(TwoWire* i2c_bus = &Wire, uint8_t i2c_addr = BMP390_ALT_I2C_ADDR);
  BMP390Sensor(unsigned long minium_period, TwoWire* i2c_bus = &Wire, uint8_t i2c_addr = BMP390_ALT_I2C_ADDR);

  bool verify() override;
  String readData() override;
  void readDataPacket(uint8_t*& packet) override;
  String decodeToCSV(uint8_t*& packet) override;
};

#endif  // BMP384_SENSOR_H