#include "SysVar.h"

#define SYSVAR_ACCESS_DELAY 100

// mutex
static SemaphoreHandle_t sysvar_mutex = xSemaphoreCreateMutex();

// System Variables
static float pico_temp_c;
static uint32_t rtc_time; 

// Access functions
#define with_sysvar_mutex(operation)                       \
  if (xSemaphoreTake(sysvar_mutex, SYSVAR_ACCESS_DELAY)) { \
    operation;                                             \
    xSemaphoreGive(sysvar_mutex);                          \
    return 0;                                              \
  } else {                                                 \
    return -1;                                             \
  }

int8_t sysvar_get_pico_temp_c(float* output) {
  with_sysvar_mutex(*output = pico_temp_c);
}

int8_t sysvar_set_pico_temp_c(float input) {
  with_sysvar_mutex(pico_temp_c = input);
}

int8_t sysvar_get_rtc_time(uint32_t* output){
  with_sysvar_mutex(*output = rtc_time)
}

int8_t sysvar_set_rtc_time(uint32_t input){
  with_sysvar_mutex(rtc_time = input); 
}