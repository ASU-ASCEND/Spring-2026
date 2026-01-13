#include "RadioStorage.h"

#include "Logger.h"

static volatile bool transmission_done_flag = true;

static void setTransmissionDoneFlag(void) { transmission_done_flag = true; }

/**
 * @brief Construct a new RadioStorage object
 *
 */
RadioStorage::RadioStorage() : Storage("Radio") {
  this->last_transmission_time = 0;
}

/**
 * @brief Initialize UART1 (Serial1)
 *
 * @return true UART1 bus was successfully initialized
 * @return false otherwise
 */
bool RadioStorage::verify() {
  this->state = this->radio.begin(RADIO_FREQ, RADIO_BW, RADIO_SF, RADIO_CR,
                                  RADIO_SYNC_WORD, RADIO_POWER,
                                  RADIO_PREAMBLE_LEN, RADIO_GAIN);

  this->radio.setPacketSentAction(setTransmissionDoneFlag);

  // turn to TX mode
  pinMode(RADIO_TX_ENABLE, OUTPUT);
  pinMode(RADIO_RX_ENABLE, OUTPUT);
  digitalWrite(RADIO_RX_ENABLE, LOW);
  digitalWrite(RADIO_TX_ENABLE, HIGH);

  return this->state == RADIOLIB_ERR_NONE;
}

/**
 * @brief Send data to the radio for transmitting
 *
 * @param data Data to transmit
 */
void RadioStorage::store(String data) {
  static const unsigned long transmission_mod = 1;
  static unsigned long transmission_count = 0;
  if (transmission_done_flag && transmission_count % transmission_mod == 0 &&
      (millis() - this->last_transmission_time) > MINIMUM_TRANSMIT_PERIOD_MS) {
    transmission_done_flag = false;

    if (this->state == RADIOLIB_ERR_NONE) {
      log_core("Transmission finished!");
    } else {
      log_core("Failed, code " + String(this->state));
    }

    this->radio.finishTransmit();

    this->state = this->radio.startTransmit(data);
    this->last_transmission_time = millis();
  }
  transmission_count++;
}

/**
 * @brief Send data to the radio for transmitting
 *
 * @param packet Packet to send
 */
void RadioStorage::storePacket(uint8_t* packet) {
  static const unsigned long transmission_mod = 1;
  static unsigned long transmission_count = 0;

  // write to packet
  if (transmission_done_flag && transmission_count % transmission_mod == 0) {
    transmission_done_flag = false;

    if (this->state == RADIOLIB_ERR_NONE) {
      log_core("Transmission finished!");
    } else {
      log_core("Failed, code " + String(this->state));
    }

    this->radio.finishTransmit();

    // length of packet will be after sync bytes (4) and sensor presence (4)
    // it is uint16_t
    uint16_t packet_len;
    memcpy(&packet_len, (packet + 8), sizeof(uint16_t));

    this->state = this->radio.startTransmit(packet, packet_len);
  }
  transmission_count++;
}