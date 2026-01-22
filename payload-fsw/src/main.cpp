/**
 * @file main.cpp
 * @brief Main functions for Core 0, responsible for reading data from sensor
 * peripherals
 */
#include <Arduino.h>

// error code framework
#include "ErrorDisplay.h"
#include "Logger.h"
#include "PayloadConfig.h"

// parent classes
#include "Sensor.h"

// include sensor headers here
#include "AS7331Sensor.h"
#include "AnalogTemp.h"
#include "BMP390Sensor.h"
#include "ENS160Sensor.h"
#include "ICM20948Sensor.h"
#include "OzoneSensor.h"
#include "PCF8523Sensor.h"
#include "SHTC3Sensor.h"
#include "TMP11xSensor.h"
#include "TempSensor.h"

// helper function definitions
int verifySensors();
void handleCommand();
int verifySensorRecovery();
String readSensorData();
uint16_t readSensorDataPacket(uint8_t* packet);
String decodePacket(uint8_t* packet);

void handleDataInterface();

// Global variables
// sensor classes
// clang-format off
// class        sensor            minimum period in ms
TempSensor      temp_sensor       (1000);
ICM20948Sensor  icm_sensor        (0);
PCF8523Sensor   rtc_sensor        (1000);
TMP11xSensor    tmp_sensor        (500,   &STRATOCORE_I2C); 

// StratoSense
AS7331Sensor    uv_sensor_out     (500, UV_I2C_ADDR);
ENS160Sensor    ens160_sensor_out (500,   &STRATOSENSE_I2C);
BMP390Sensor    bmp_sensor_out    (500,   &STRATOSENSE_I2C);
TMP11xSensor    tmp_sensor_out    (500,   &STRATOSENSE_I2C); 
SHTC3Sensor     shtc3_sensor_out  (500,   &STRATOSENSE_I2C);
OzoneSensor     ozone_sensor_out  (500);
AnalogTemp      analog_temp_out   (500);
// clang-format on

// sensor array
Sensor* sensors[] = {&temp_sensor,      &icm_sensor,     &rtc_sensor,
                     &tmp_sensor,       &uv_sensor_out,  &ens160_sensor_out,
                     &bmp_sensor_out,   &tmp_sensor_out, &shtc3_sensor_out,
                     &ozone_sensor_out, &analog_temp_out};

const int sensors_len = sizeof(sensors) / sizeof(sensors[0]);

String header_condensed = "";

// global variables for main

// Global variables shared with core 1

queue_t qt;

uint32_t time_paused;
uint32_t max_pause_duration = 60'000 * 2;

// char qt_entry[QT_ENTRY_SIZE];

/**
 * @brief Setup for core 0
 *
 */
void setup() {
  // multicore setup
  queue_init(&qt, QT_ENTRY_SIZE, QT_MAX_SIZE);
  ErrorDisplay::instance().addCode(Error::NONE);  // for safety

  pinMode(BAD_I2C0_SDA_PIN, INPUT);
  pinMode(BAD_I2C0_SCL_PIN, INPUT);

  // setup i2c1
  Wire1.setSCL(I2C1_SCL_PIN);
  Wire1.setSDA(I2C1_SDA_PIN);

  // setup i2c0
  Wire.setSCL(I2C0_SCL_PIN);
  Wire.setSDA(I2C0_SDA_PIN);

  Wire.begin();
  Wire1.begin();

  // start serial
  Serial.begin(115200);
  // while (!Serial)  // remove before flight
  //
  log_core("setup begin");

  // setup heartbeat pins
  pinMode(HEARTBEAT_PIN_0, OUTPUT);

  // verify sensors
  // recovery config for sensors
  for (int i = 0; i < sensors_len; i++) {
    sensors[i]->recoveryConfig(5, 1000);
  }

  int verified_count = verifySensorRecovery();

  if (verified_count == 0) {
    log_core("All sensor communications failed");
    ErrorDisplay::instance().addCode(Error::CRITICAL_FAIL);
    while (1) {
      ErrorDisplay::instance().toggle();
      log_core("Error");
      delay(1000);
    }
  } else {
    log_core("At least one sensor works, continuing");
    if (verified_count < 5) {
      ErrorDisplay::instance().addCode(Error::LOW_SENSOR_COUNT);
    }
  }

  pinMode(ON_BOARD_LED_PIN, OUTPUT);
  log_core("Setup done.");
}

bool was_dumping = false;
// loop counter
unsigned int it = 0;
/**
 * @brief Loop for core 0, handling sensor reads
 *
 */
void loop() {
  // toggle error display
  ErrorDisplay::instance().toggle();

  // toggle heartbeats
  it++;
  digitalWrite(HEARTBEAT_PIN_0, (it & 0x1));

  // start print line with iteration number
  log_core("it: " + String(it) + "\t");

  // build csv row
  uint8_t packet[QT_ENTRY_SIZE];
  // for (int i = 0; i < QT_ENTRY_SIZE; i++) packet[i] = 0; // useful for
  // debugging
  uint16_t packet_len = readSensorDataPacket(packet);

  String data_str = decodePacket(packet);
  log_core("Data: " + data_str);

  // print csv row
  // log_data(csv_row);
  log_data_raw(packet, packet_len);

  // send data to core1
  // queue_add_blocking(&qt, packet);
  queue_try_add(&qt, packet);

  delay(500);  // remove before flight

  digitalWrite(ON_BOARD_LED_PIN, (it & 0x1));  // toggle light with iteration
}

/**
 * @brief Uses Device abstraction to verify sensors
 * Incompatible with initial header generation
 *
 * @return int Number of sensors verified
 */
int verifySensorRecovery() {
  int count = 0;
  for (int i = 0; i < sensors_len; i++) {
    if (sensors[i]->attemptConnection()) {
      count++;
    }
  }

  log_core("Pin Verification Results:");
  for (int i = 0; i < sensors_len; i++) {
    log_core((sensors[i]->getDeviceName()) + ": " +
             (sensors[i]->getVerified()
                  ? "Successful in Communication"
                  : "Failure in Communication (check wirings and/ or pin "
                    "definitions)"));
  }
  log_core("");
  return count;
}

/**
 * @brief Reads sensor data into a packet byte array
 *
 * @param packet Pointer to the packet array
 */
uint16_t readSensorDataPacket(uint8_t* packet) {
  // set sync bytes
  uint8_t* temp_packet = packet;
  std::copy(SYNC_BYTES, SYNC_BYTES + sizeof(SYNC_BYTES), temp_packet);
  temp_packet += sizeof(SYNC_BYTES);

  uint32_t sensor_id = 0;
  uint16_t packet_len = 0;
  temp_packet += sizeof(sensor_id) + sizeof(packet_len);

  // build packet
  // millis()
  uint32_t now = millis();
  std::copy((uint8_t*)(&now), (uint8_t*)(&now) + sizeof(now), temp_packet);
  temp_packet += sizeof(now);
  sensor_id = (sensor_id << 1) | 1;
  // rest of the packet
  for (int i = 0; i < sensors_len; i++) {
    if (sensors[i]->attemptConnection()) {
      sensors[i]->getDataPacket(sensor_id, temp_packet);
    } else {
      sensor_id <<= 1;
    }
  }

  // calc data len
  packet_len = (temp_packet - packet) + 1;  // + 1 for checksum
  log_core("Packet Len: " + String(packet_len));

  // write sensor_id
  temp_packet = packet + sizeof(SYNC_BYTES);
  std::copy((uint8_t*)(&sensor_id), (uint8_t*)(&sensor_id) + sizeof(sensor_id),
            temp_packet);

  // write data len
  temp_packet += sizeof(sensor_id);
  std::copy((uint8_t*)(&packet_len),
            (uint8_t*)(&packet_len) + sizeof(packet_len), temp_packet);

  // calculate checksum with sum complement parity
  int8_t checksum = 0;
  for (size_t i = 0; i < packet_len - 1; i++) {
    checksum += packet[i];
  }
  *(packet + packet_len - 1) = -checksum;

  return packet_len;
}

/**
 * @brief Decodes the packet to a CSV row
 *
 * @param packet Pointer to the packet array
 * @return String The resulting CSV row
 */
String decodePacket(uint8_t* packet) {
  uint8_t* temp_packet = packet;

  uint32_t sync_bytes = *((uint32_t*)temp_packet);
  temp_packet += sizeof(sync_bytes);
  uint32_t sensor_id = *((uint32_t*)temp_packet);
  temp_packet += sizeof(sensor_id);
  uint16_t packet_len = *((uint16_t*)temp_packet);
  temp_packet += sizeof(packet_len);

  // start with sensor_id in a cell in Hex
  String csv_row = String(sensor_id, HEX) + ",";

  uint32_t sensor_id_temp = sensor_id;
  uint8_t id_offset = 0;
  for (int i = 0; i < 32; i++) {
    if (sensor_id_temp & (1 << i)) {
      id_offset = i;
    }
  }

  // millis decode
  // the words aren't aligned because of the uint16_t len
  // so casting will crash the pico
  uint32_t r_now;
  memcpy(&r_now, temp_packet, sizeof(uint32_t));

  temp_packet += sizeof(r_now);
  csv_row += String(r_now) + ",";

  int curr_offset = id_offset - 1;
  while (curr_offset >= 0) {
    if (sensor_id & (1 << curr_offset)) {
      // log_core("\tDecoding " + sensors[id_offset - curr_offset -
      // 1]->getDeviceName());
      csv_row += sensors[id_offset - curr_offset - 1]->decodeToCSV(temp_packet);
    } else if (sensors[id_offset - curr_offset - 1]->getVerified()) {
      csv_row += sensors[id_offset - curr_offset - 1]->readEmpty();
    } else {
    }
    curr_offset--;
  }

  // check parity
  uint8_t sum = 0;
  for (uint16_t i = 0; i < packet_len - 1; i++) {
    sum += packet[i];
  }
  sum += *(int8_t*)(packet + packet_len - 1);
  log_core("Sum = " + String(sum));

  return csv_row;
}

/**
 * @brief Read data from each verified Sensor
 *
 * @return String Complete CSV row for iteration
 */
String readSensorData() {
  String csv_row = header_condensed + "," + String(millis()) + ",";
  for (int i = 0; i < sensors_len; i++) {
    if (sensors[i]->attemptConnection()) {
      csv_row += sensors[i]->getDataCSV();
    }
  }
  return csv_row;
}

//-------------------------------------
// Core 1 Calls
//-------------------------------------
// declarations - definitions are in main1.cpp
void real_setup1();
void real_loop1();

void setup1() { real_setup1(); }

void loop1() { real_loop1(); }

//-------------------------------------