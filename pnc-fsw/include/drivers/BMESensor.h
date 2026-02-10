#ifndef BME_SENSOR_H
#define BME_SENSOR_H

#include "Adafruit_BME680.h"
#include "Sensor.h"
#include "Wire.h"

struct BMESensorData
{
    float BMETemp;
    uint32_t BMEPressure;
    float BMEHumidity;
    uint32_t BMEGasResistance;
};

/**
 * @brief Implementation of a Sensor for the onboard temperature sensor
 *
 */
class BMESensor : public Sensor
{
private:
    Adafruit_BME680 bme;

public:
    BMESensor();
    bool verify() override;
    void readToSysVar() override;
};

#endif
