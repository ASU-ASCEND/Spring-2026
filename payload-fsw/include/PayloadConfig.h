/**
 * @file PayloadConfig.h
 * @brief Central location for pinout and config defines
 *
 * All pin definitions must include PIN in their name to allow the conflict
 * checker to catch pin conflits ex: SD_CS_PIN instead of just SD_CS
 *
 */

#ifndef PAYLOAD_CONFIG_H
#define PAYLOAD_CONFIG_H

#include <Wire.h>

/** @brief Error display bit 2 */
#define ERROR_PIN_2 22
/** @brief Error display bit 1 */
#define ERROR_PIN_1 15
/** @brief Error display bit 0 */
#define ERROR_PIN_0 14

/** @brief MTK3339 GPS CS Pin*/
// #define MTK3339_CS_PIN 20
#define SERIAL2_RX_PIN 9
#define SERIAL2_TX_PIN 8
#define GPS_I2C 1

/** @brief UV sensor I2C Addresses */
// [1, 1, 1, 0, 1, A1, A0]
#define UV_I2C_ADDR 0x74

/** @brief Ozone I2C address */
#define OZONE_I2C_ADDR 0x73

/** @brief I2C1 pins */
#define I2C1_SDA_PIN 10
#define I2C1_SCL_PIN 11

/** @brief Default I2C0 pins for disabling */
#define BAD_I2C0_SDA_PIN 4
#define BAD_I2C0_SCL_PIN 5

/** @brief I2C0 pins */
#define I2C0_SDA_PIN 8
#define I2C0_SCL_PIN 9

/** @brief Default I2C Address for TMP117 */
#define TMP117_I2C_ADDR 0x48

/** @brief ADC Pin for Thermistor Readings */
#define THERMISTOR_PIN 28

/** @brief StratoSense board I2C Bus */
#define STRATOSENSE_I2C Wire1

/** @brief StatoCore board I2C Bus */
#define STRATOCORE_I2C Wire

// storages
/** @brief Toggle sending packets vs C strings over transfer queue */
#define STORING_PACKETS 1

// for radio
/// Radio pinouts
#define RADIO_NSS 1
#define RADIO_DIO0 2
#define RADIO_RESET 0
#define RADIO_DIO1 3
#define RADIO_TX_ENABLE 26
#define RADIO_RX_ENABLE 27

#define RADIO_FREQ 915.0
#define RADIO_BW 125.0
#define RADIO_SF 9
#define RADIO_CR 7
#define RADIO_SYNC_WORD RADIOLIB_SX127X_SYNC_WORD
#define RADIO_POWER 17
#define RADIO_PREAMBLE_LEN 8
#define RADIO_GAIN 0

#define MINIMUM_TRANSMIT_PERIOD_MS 10000  // 10s

/** @brief Radio SPI  */
#define RADIO_SPI SPI

// spi1
/** @brief SPI1 MISO Pin */
#define SPI1_MISO_PIN 12
/** @brief SPI1 SCK Pin */
#define SPI1_SCK_PIN 14
/** @brief SPI1 MOSI Pin */
#define SPI1_MOSI_PIN 15

/** @brief SD Card SPI Toggle */
#define SD_SPI1 0
/** @brief SD Card SPI CS Pin */
#define SD_CS_PIN 17

/** @brief Flash memory SPI Toggle */
#define FLASH_SPI1 1
/** @brief Flash chip SPI CS Pin */
#define FLASH_CS_PIN 13

// main pin definitions
/** @brief Built-in LED Pin */
#define ON_BOARD_LED_PIN 25
/** @brief Core 0 Heartbeat Pin */
#define HEARTBEAT_PIN_0 21
/** @brief Core 1 Heartbeat Pin */
#define HEARTBEAT_PIN_1 20

// multicore transfer queue
#define QT_ENTRY_SIZE 500
#define QT_MAX_SIZE 10

// packet properties
const uint8_t SYNC_BYTES[] = {'A', 'S', 'U', '!'};

// temporary toggle macros for testing
#define PACKET_SYSTEM_TESTING 1

#endif