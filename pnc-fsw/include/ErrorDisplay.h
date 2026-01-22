#ifndef ERROR_DISPLAY_H
#define ERROR_DISPLAY_H

#include <Arduino.h>
#include <FreeRTOS.h>

#include "HardwareConfig.h"
#include "Logger.h"
#include "semphr.h"

#define ERROR_MUTEX_DELAY pdMS_TO_TICKS(50)

/**
 * Error codes:
 * Smallest will be displayed as the binary value 7 - (enum value)
 * ex. CRITICAL_FAIL (enum value of 0) will be all 3 leds blinking (7 - 0 = 7 or
 * 0b111) Max 7 errors (1 led should always be blinking) NONE will always be 1
 */
typedef enum {
  // no sensors or no storage
  CRITICAL_FAIL = 0,
  // triggered if SD card verify function returns false or if an SD card write
  // fails
  SD_CARD_FAIL,
  // triggered for less than 5 sensors verified
  LOW_SENSOR_COUNT,
  // triggered if SD card has multiple files TODO fix trigger to be more useful
  POWER_CYCLED,
  // default state, lowest priority
  NONE
} Error;

/**
 * @brief Singleton class for the 3 GPIO LED Error Display. Uses mutex
 * protection for data and GPIO access BUT DOES NOT ENFORCE IT.
 *
 */
class ErrorDisplay {
 private:
  // mutex_t error_display_mutex;
  SemaphoreHandle_t error_display_mutex;
  int pin_level;
  Error code;

  ErrorDisplay() {
    // mutex_init(&error_display_mutex);
    error_display_mutex = xSemaphoreCreateMutex();
    this->pin_level = 1;
    this->code = NONE;
    pinMode(ERROR_2_PIN, OUTPUT);
    pinMode(ERROR_1_PIN, INPUT);
    pinMode(ERROR_0_PIN, INPUT);
  }

 public:
  /**
   * @brief Accesses the only instance of ErrorDisplay (Singleton)
   *
   * @return ErrorDisplay& Instance of ErrorDisplay
   */
  static ErrorDisplay& instance() {
    static ErrorDisplay only_instance;
    return only_instance;
  }

  /**
   * @brief Sets the error code to the given value if the given error is of a
   * higher priority then the current error
   *
   * @param e The error code to display
   */
  void addCode(Error e) {
    // mutex_enter_blocking(&error_display_mutex);
    bool has_mutex = false;
    if (xSemaphoreTake(error_display_mutex, ERROR_MUTEX_DELAY) == pdTRUE) {
      has_mutex = true;
    } else {
      log_task_error("Bypassing ErrorDisplay mutex");
    }

    if (e < this->code) {
      this->code = e;
    }

    // mutex_exit(&error_display_mutex);
    if (has_mutex) xSemaphoreGive(error_display_mutex);
  }

  /**
   * @brief Toggles the level of the error display
   *
   */
  void toggle() {
    // mutex_enter_blocking(&error_display_mutex);
    bool has_mutex = false;
    if (xSemaphoreTake(error_display_mutex, ERROR_MUTEX_DELAY) == pdTRUE) {
      has_mutex = true;
    } else {
      log_task_error("Bypassing ErrorDisplay mutex");
    }

    this->pin_level = !(this->pin_level);

    uint8_t display_code = 7 - this->code;  // 0 is highest

    if (this->code == Error::NONE) display_code = 0b001;

    digitalWrite(ERROR_2_PIN, this->pin_level && (display_code & 0b100));
    digitalWrite(ERROR_1_PIN, this->pin_level && (display_code & 0b010));
    digitalWrite(ERROR_0_PIN, this->pin_level && (display_code & 0b001));

    // mutex_exit(&error_display_mutex);
    if (has_mutex) xSemaphoreGive(error_display_mutex);
  }
};

#endif