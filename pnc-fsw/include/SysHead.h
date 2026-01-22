#ifndef SYS_HEAD_H
#define SYS_HEAD_H

// Arduino SDK
#include <Arduino.h>

// global config / utility headers
#include "ErrorDisplay.h"
#include "HardwareConfig.h"
#include "Logger.h"
#include "SysVar.h"

// freeRTOS
#define TASK_PRIORITY_DEFAULT (configMAX_PRIORITIES / 2)

#endif