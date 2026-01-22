#include <Arduino.h>
#include <FreeRTOS.h>

#include "SysHead.h"
#include "task.h"

// tasks
#include "tasks/can_manage.h"
#include "tasks/monitor.h"
#include "tasks/watchdog.h"


void setup() {
  Serial.begin(115200);
  while (!Serial);

  // global setups
  // setup better adc res
  analogReadResolution(PICO_TEMP_ADC_RES);

  // start tasks
  watchdog_task_init(); 
  monitor_task_init();
  can_task_init();

  log_task("Setup complete.");
}

void loop() {

  // update pico temp
  float temp = analogReadTemp();
  sysvar_set_pico_temp_c(temp);

  log_task("Sending " + String(temp));

  delay(500);
}
