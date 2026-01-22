#include "tasks/can_manage.h"

#include <Arduino.h>
#include <FreeRTOS.h>

#include "SysHead.h"
#include "task.h"

TaskHandle_t can_task_handle;

void can_task(void* params) {
  while (1) {
    log_task("Hello");

    delay(500);
  }
}

void can_task_init() {
  // task setup

  // task start
  BaseType_t res = xTaskCreate(can_task, "CAN", 1024, nullptr,
                               TASK_PRIORITY_DEFAULT, &can_task_handle);
  log_task("CAN Task Started.");
}