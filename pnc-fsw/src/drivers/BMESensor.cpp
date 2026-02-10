#include "drivers/BMESensor.h"
#include "SysHead.h"
#include "Wire.h"

BMESensor::BMESensor() : Sensor("BME")
{
    bme = Adafruit_BME680(&Wire);
}

bool BMESensor::verify()
{
    return bme.begin();
}

void BMESensor::readToSysVar()
{
    BMESensorData sensor_data;
    bme.performReading();

    sensor_data.BMETemp = bme.temperature;
    sensor_data.BMEPressure = bme.pressure;
    sensor_data.BMEHumidity = bme.humidity;
    sensor_data.BMEGasResistance = bme.gas_resistance;

    sysvar_set_bme_data(&sensor_data);
}