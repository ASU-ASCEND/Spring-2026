#include "tasks/can_manage.h"

#include <Arduino.h>

#include <FreeRTOS.h>
#include "task.h"

#include "SysHead.h"

static TaskHandle_t can_task_handle; 

void can_task(void * params){

  while(1){
    log_task("Hello"); 

    delay(500); 
  }
}

void can_task_init(){
  // task setup 

  // task start
  BaseType_t res = xTaskCreate(can_task, 
                              "CAN", 
                              1024, 
                              nullptr, 
                              configMAX_PRIORITIES, 
                              &can_task_handle);
}