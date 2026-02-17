#include "drivers/PicoTempSensor.h"

#include "SysHead.h"

/**
 * @brief Construct a new Temp Sensor object
 *
 */
PicoTempSensor::PicoTempSensor() : Sensor("PicoTemp") {}

/**
 * @brief Returns if sensor can be reached, the temperature sensor is on the
 * pico so it is assumed to be always true, implemented only for compatibility
 * with parent
 *
 * @return true always
 * @return false never
 */
bool PicoTempSensor::verify()
{
  analogReadResolution(PICO_TEMP_ADC_RES);
  return true;
}

void PicoTempSensor::readToSysVar()
{
  float temp = analogReadTemp();
  sysvar_set_pico_temp_c(temp);
}
