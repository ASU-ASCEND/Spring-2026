#include <Arduino.h>
#include <FreeRTOS.h>
#include "task.h"

#include "SysHead.h"

// tasks 
#include "tasks/monitor.h"
#include "tasks/can_manage.h"

ErrorDisplay& errorDisplay = ErrorDisplay::instance();

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // global setups 
  // setup better adc res
  analogReadResolution(PICO_TEMP_ADC_RES);

  // start tasks
  monitor_task_init();
  can_task_init(); 

  log_task("Setup complete.");
}

void loop() {
  errorDisplay.toggle();

  // update pico temp
  float temp = analogReadTemp();
  sysvar_set_pico_temp_c(temp);

  log_task("Sending " + String(temp));

  delay(500);
}
