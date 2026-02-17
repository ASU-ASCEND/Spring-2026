#ifndef SYS_VAR_H
#define SYS_VAR_H

#include <Arduino.h>
#include <FreeRTOS.h>
#include <drivers/INASensor.h>

#include "semphr.h"

// Access functions
int8_t sysvar_get_pico_temp_c(float* output);
int8_t sysvar_set_pico_temp_c(float input);

int8_t sysvar_get_rtc_time(uint32_t* output);
int8_t sysvar_set_rtc_time(uint32_t input);

int8_t sysvar_set_ina_data(INASensorData *ina_sensor_data);
int8_t sysvar_get_ina_data(INASensorData *ina_sensor_data);
#endif  // SYS_VAR_H