#include <Arduino.h>
#include <FreeRTOS.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>

#include "ErrorDisplay.h"
#include "SysHead.h"
#include "task.h"

// tasks
#include "tasks/can_manage.h"
#include "tasks/monitor.h"
#include "tasks/watchdog.h"

// sensors
#include "drivers/BMESensor.h"
#include "drivers/GPSSensor.h"
#include "drivers/INASensor.h"
#include "drivers/PicoTempSensor.h"
#include "drivers/RTCSensor.h"
#include "drivers/Sensor.h"

PicoTempSensor pico_temp_sensor;
RTCSensor rtc_sensor;
BMESensor bme_sensor;
GPSSensor gps_sensor;
INASensor ina_sensor;

Sensor* sensors[] = {&pico_temp_sensor, &rtc_sensor, &bme_sensor, &ina_sensor,
                     &gps_sensor};
size_t sensors_len = sizeof(sensors) / sizeof(sensors[0]);

// declarations
void sysvar_update();
void store_data();
void sd_setup();
bool sd_status = false;
String filename = "";

struct __attribute__((packed)) Packet {
  uint32_t sync_bytes = 0;
  uint8_t id = 0;
  uint8_t length = sizeof(uint32_t) + 3 * sizeof(uint8_t) + sizeof(float) +
                   sizeof(BMESensorData) + sizeof(INASensorData) +
                   sizeof(GPSSensorData);
  float temp_data;
  uint32_t rtc_time;
  BMESensorData bme_data;
  INASensorData ina_data;
  GPSSensorData gps_data;
  int8_t checksum;
};

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(100);

  // Initialize I2C bus once before verifying any I2C sensors.
  Wire.begin();

  log_task("ASCEND PnC FSW");

  // sensor setups
  for (int i = 0; i < sensors_len; i++) {
    log_task("Verifying " + sensors[i]->getSensorName() + "...");
    if (sensors[i]->verify()) {
      log_task("Success.");
    } else {
      log_task("Failure.");
    }
  }

  // sd setup
  sd_setup();

  // start tasks
  watchdog_task_init();
  monitor_task_init();
  can_task_init();

  log_task("Setup complete.");
}

void sd_setup() {
  if (SD.begin(SD_PIN)) {
    sd_status = true;
    int num = 0;
    while (SD.exists("data" + String(num) + ".bin")) num++;

    filename = "data" + String(num) + ".bin";
    File file = SD.open(filename, FILE_WRITE);
    if (!file) {
      ErrorDisplay::instance().addCode(Error::SD_CARD_FAIL);
      sd_status = false;
      SD.end();
    }
    file.close();
  } else {
    ErrorDisplay::instance().addCode(Error::SD_CARD_FAIL);
  }
}

void loop() {
  sysvar_update();
  store_data();
  delay(500);
}

void sysvar_update() {
  // update pico temp
  log_task("Starting SysVar Update...");

  for (int i = 0; i < sensors_len; i++) {
    sensors[i]->readToSysVar();
  }

  log_task("Done.");
}

void store_data() {
  if (sd_status == false) {
    sd_setup();
    return;
  }

  float pico_temp_c;
  uint32_t rtc_time;
  BMESensorData bme_data;
  INASensorData ina_data;
  GPSSensorData gps_data;
  Packet packet;

  int8_t sum = 0;

  sysvar_get_pico_temp_c(&pico_temp_c);
  sysvar_get_rtc_time(&rtc_time);
  sysvar_get_bme_data(&bme_data);
  sysvar_get_ina_data(&ina_data);
  sysvar_get_gps_data(&gps_data);

  packet.temp_data = pico_temp_c;
  packet.rtc_time = rtc_time;
  packet.bme_data = bme_data;
  packet.ina_data = ina_data;
  packet.gps_data = gps_data;

  uint8_t* pos = (uint8_t*)&packet;

  for (size_t i = 0; i < packet.length; i++) {
    sum += *pos;
    pos++;
  }
  packet.checksum = -sum;  // calculate checksum with sum complement parity
  File output = SD.open(filename, FILE_WRITE);
  if (!output) {
    ErrorDisplay::instance().addCode(Error::SD_CARD_FAIL);
    sd_status = false;
    SD.end();
  }
  output.write((uint8_t*)&packet, packet.length);
  output.close();
}
