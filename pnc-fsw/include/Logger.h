#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <FreeRTOS.h>
#include <stdarg.h>

#include "task.h"

static inline size_t log_printf(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);

  size_t res = Serial.printf(fmt, args);

  va_end(args);
  return res;
}

static inline void log_task(String str) {
  char* task_name = pcTaskGetName(NULL);
  Serial.print("[" + String(task_name) + "] " + str + "\n");
}

static inline void log_task_error(String str) {
  char* task_name = pcTaskGetName(NULL);
  Serial.print("[ERROR - " + String(task_name) + "] " + str + "\n");
}

static inline void log_data_raw(const uint8_t* packet, const uint8_t len) {
  Serial.write((const char*)packet, len);
}

static inline void log_data_bytes(const uint8_t* packet, const uint8_t len) {
  String packet_as_hex = "";
  for (size_t i = 0; i < len; i++) {
    packet_as_hex += String(packet[i], HEX) + " ";
  }
  packet_as_hex += "\n";
  Serial.print(packet_as_hex);
}

static inline void log_data(String data) {
  Serial.print("[Data] " + data + "\n");
}

#endif  // LOGGER_H