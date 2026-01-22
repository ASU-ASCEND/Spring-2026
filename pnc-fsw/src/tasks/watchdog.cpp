#include "tasks/watchdog.h"

#include <FreeRTOS.h>
#include "task.h"

#include "SysHead.h"

#include "hardware/watchdog.h"

#define WATCHDOG_INTERVAL_MS 5000

static ErrorDisplay& error_display = ErrorDisplay::instance(); 

TaskHandle_t watchdog_task_handle;

static void watchdog_task(void * params){

  watchdog_enable(WATCHDOG_INTERVAL_MS, true); 

  while(1){
    error_display.toggle(); 
    
    watchdog_update(); 

    delay(500); 
  }

}

void watchdog_task_init(){
  // setup task 

  // start task 
  BaseType_t res = xTaskCreate(watchdog_task, "WATCHDOG", 256, nullptr, TASK_PRIORITY_DEFAULT, &watchdog_task_handle);

  log_task("Watchdog task started."); 
}