#include "tasks/watchdog.h"

#include "SysHead.h"
#include "hardware/watchdog.h"
#include "pico/mutex.h"

#define WATCHDOG_INTERVAL_MS 5000

#define WATCHDOG_CONNECTED_TASKS 2
#define WATCHDOG_INTERTASK_CHECK_PERIOD_MS 1000 * 60  // 1 minute

#if WATCHDOG_CONNECTED_TASKS > 0
static bool heartbeats[WATCHDOG_CONNECTED_TASKS];
#endif
static mutex_t heartbeats_mutex;

static ErrorDisplay& error_display = ErrorDisplay::instance();

static uint32_t last_check = 0;

void watchdog_task() {
  watchdog_enable(WATCHDOG_INTERVAL_MS, true);

  log_task("Watchdog it");
  error_display.toggle();

  watchdog_update();

  // check intertask
  mutex_enter_blocking(&heartbeats_mutex);
  if (millis() - last_check > WATCHDOG_INTERTASK_CHECK_PERIOD_MS) {
    // check heartbeats
    for (size_t i = 0; i < WATCHDOG_CONNECTED_TASKS; i++) {
      // if a heartbeat hasn't toggled since last check
      if (heartbeats[i] == false) {
        // watchdog freeze
        log_task("Watchdog freeze");
        while (1);
      }
      // reset heartbeat
      heartbeats[i] = false;
    }
  }
  mutex_exit(&heartbeats_mutex);

  delay(500);
}

static bool watchdog_intertask_update_disabled = true;
void watchdog_intertask_update(uint8_t id) {
  if (watchdog_intertask_update_disabled) return;
  if (mutex_try_enter_block_until(&heartbeats_mutex,
                                  make_timeout_time_ms(100))) {
    heartbeats[id] = true;
    mutex_exit(&heartbeats_mutex);
  }
}

void watchdog_task_init() {
  // setup task
  mutex_init(&heartbeats_mutex);
  watchdog_intertask_update_disabled = false;  // toggle

  log_task("Watchdog task started.");
}