#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

/**
 * @brief Parent class for sensor objects
 *
 */
class Sensor {
 protected:
  String sensor_name;

 public:
  Sensor(String sensor_name) : sensor_name(sensor_name) {}

  /**
   * @brief Verifies if the sensor is connected and working
   *
   * @return true, if connected
   * @return false, if not connected
   */
  virtual bool verify() = 0;

  /**
   * @brief Reads sensor data and updates corresponding SysVar
   *
   */
  virtual void readToSysVar() = 0;

  /**
   * @brief Returns the sensor name
   *
   * @return String
   */
  const String getSensorName() const { return this->sensor_name; }
};

#endif
