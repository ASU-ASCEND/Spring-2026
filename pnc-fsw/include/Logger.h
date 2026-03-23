#ifndef LOGGER_H
#define LOGGER_H

#include <Arduino.h>
#include <stdarg.h>

#define log_printf(...) Serial.printf(__VA_ARGS__)

#define log_task_printf(fmt, ...) \
  Serial.printf("[Core %d] " fmt, get_core_num(), ##__VA_ARGS__)

static inline void log_task(String str) {
  Serial.print("[" + String(get_core_num()) + "] " + str + "\n");
}

static inline void log_task_error(String str) {
  Serial.print("[ERROR - " + String(get_core_num()) + "] " + str + "\n");
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