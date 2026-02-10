#include "tasks/monitor.h"

#include "SysHead.h"
#include <drivers/BMESensor.h>
TaskHandle_t monitor_task_handle;

static void monitor_task(void *params)
{
  while (1)
  {
    float pico_temp_c;
    int8_t res = sysvar_get_pico_temp_c(&pico_temp_c);

    log_task("Pico Temp: " + String(pico_temp_c) + "(" + String(res) + ")");

    uint32_t rtc_time;
    res = sysvar_get_rtc_time(&rtc_time);
    log_task("RTC Time: " + String(rtc_time) + "(" + String(res) + ")");

    BMESensorData bme_data;
    res = sysvar_get_bme_data(&bme_data);
    log_task("BME Temperature: " + String(bme_data.BMETemp) + "(" + String(res) + ")");
    log_task("BME Pressure: " + String(bme_data.BMEPressure) + "(" + String(res) + ")");
    log_task("BME Humidity: " + String(bme_data.BMEHumidity) + "(" + String(res) + ")");
    log_task("BME Gas Sensor: " + String(bme_data.BMEGasResistance) + "(" + String(res) + ")");

    delay(500);
  }
}

void monitor_task_init()
{
  // do task setup

  // start
  BaseType_t res = xTaskCreate(monitor_task, "MONITOR", 1024, nullptr,
                               TASK_PRIORITY_DEFAULT, &monitor_task_handle);

  log_task("CAN Task Started.");
}
