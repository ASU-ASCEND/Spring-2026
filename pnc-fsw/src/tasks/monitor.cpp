#include "tasks/monitor.h"

#include "SysHead.h"

static TaskHandle_t monitor_task_handle;

static void monitor_task(void * params) {
  while (1) {
    float pico_temp_c;
    int8_t res = sysvar_get_pico_temp_c(&pico_temp_c);

    log_task("Pico Temp: " + String(pico_temp_c) + "(" + String(res) + ")");

    delay(500); 
  }
}

void monitor_task_init() {
  // do task setup 


  // start 
  BaseType_t res = xTaskCreate(monitor_task, 
                              "MONITOR", 
                              1024,
                              nullptr,
                              configMAX_PRIORITIES / 2,
                              &monitor_task_handle);
}
