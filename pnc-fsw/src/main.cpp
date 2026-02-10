#include <Arduino.h>
#include <FreeRTOS.h>

#include "SysHead.h"
#include "task.h"

// tasks
#include "tasks/can_manage.h"
#include "tasks/monitor.h"
#include "tasks/watchdog.h"

// sensors
#include "drivers/PicoTempSensor.h"
#include "drivers/RTCSensor.h"
#include "drivers/Sensor.h"
#include "drivers/BMESensor.h"

PicoTempSensor pico_temp_sensor;
RTCSensor rtc_sensor;
BMESensor bme_sensor;

Sensor *sensors[] = {&pico_temp_sensor, &rtc_sensor, &bme_sensor};
size_t sensors_len = sizeof(sensors) / sizeof(sensors[0]);

// declarations
void sysvar_update();

void setup()
{
  Serial.begin(115200);
  while (!Serial)
    delay(100);

  log_task("ASCEND PnC FSW");

  // sensor setups
  for (int i = 0; i < sensors_len; i++)
  {
    log_task("Verifying " + sensors[i]->getSensorName() + "...");
    if (sensors[i]->verify())
    {
      log_task("Success.");
    }
    else
    {
      log_task("Failure.");
    }
  }

  // start tasks
  watchdog_task_init();
  monitor_task_init();
  can_task_init();

  log_task("Setup complete.");
}

void loop()
{
  sysvar_update();

  delay(500);
}

void sysvar_update()
{
  // update pico temp
  log_task("Starting SysVar Update...");

  for (int i = 0; i < sensors_len; i++)
  {
    sensors[i]->readToSysVar();
  }

  log_task("Done.");
}