#ifndef WATCHDOG_H
#define WATCHDOG

#include <stdint.h>

#define WATCHDOG_MAIN_TASK_ID 0
#define WATCHDOG_MONITOR_TASK_ID 1

void watchdog_task();
void watchdog_task_init();
void watchdog_intertask_update(uint8_t id);

#endif