#include "tasks/watchdog.h"

#include <FreeRTOS.h>

#include "SysHead.h"
#include "hardware/watchdog.h"
#include "task.h"

#define WATCHDOG_INTERVAL_MS 5000

#define WATCHDOG_CONNECTED_TASKS 2
#define WATCHDOG_INTERTASK_CHECK_PERIOD_MS 1000 * 60 // 1 minute

#if WATCHDOG_CONNECTED_TASKS > 0 
static bool heartbeats[WATCHDOG_CONNECTED_TASKS]; 
#endif 
static SemaphoreHandle_t heartbeats_mutex; 

static ErrorDisplay& error_display = ErrorDisplay::instance();

TaskHandle_t watchdog_task_handle;

static uint32_t last_check = 0; 

static void watchdog_task(void* params) {
  watchdog_enable(WATCHDOG_INTERVAL_MS, true);

  while (1) {
    error_display.toggle();

    watchdog_update();

    // check intertask 
    if(xSemaphoreTake(heartbeats_mutex, portMAX_DELAY)){
      if(millis() - last_check > WATCHDOG_INTERTASK_CHECK_PERIOD_MS){
          // check heartbeats 
          for(size_t i = 0; i < WATCHDOG_CONNECTED_TASKS; i++){
              // if a heartbeat hasn't toggled since last check 
              if(heartbeats[i] == false){
                  // watchdog freeze
                  while(1); 
              }
              // reset heartbeat
              heartbeats[i] = false; 
          }
      }
      xSemaphoreGive(heartbeats_mutex); 
    }

    delay(500);
  }
}

void watchdog_intertask_kick(uint8_t id){
  if(xSemaphoreTake(heartbeats_mutex, pdMS_TO_TICKS(100))){
    heartbeats[id] = true; 
    xSemaphoreGive(heartbeats_mutex); 
  }
}

void watchdog_task_init() {
  // setup task
  heartbeats_mutex = xSemaphoreCreateMutex(); 

  // start task
  BaseType_t res = xTaskCreate(watchdog_task, "WATCHDOG", 256, nullptr,
                               TASK_PRIORITY_DEFAULT, &watchdog_task_handle);

  log_task("Watchdog task started.");
}