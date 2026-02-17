#include "drivers/INASensor.h"
#include "SysHead.h"
#include "Wire.h"


INASensor::INASensor() : Sensor("INA") {}



/**
 * @brief Verifies the connection to the INA sensor.
 *
 * This function initializes the INA sensor object and checks whether the
 * sensor is successfully detected on the I2C bus.
 *
 * @return true If the sensor is successfully initialized.
 * @return false If the sensor fails to initialize or is not detected.
 */
bool INASensor::verify() 
{
  ina = Adafruit_INA260();
  return ina.begin();
}

void INASensor::readToSysVar()
{
  INASensorData ina_data;

  ina_data.INACurrent = ina.readCurrent();
  ina_data.INABusVoltage = ina.readBusVoltage();
  ina_data.INAPower = ina.readPower();
  sysvar_set_ina_data(&ina_data);
}



