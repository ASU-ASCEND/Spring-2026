#ifndef RADIO_STORAGE_H
#define RADIO_STORAGE_H

#include <Arduino.h>
#include <RadioLib.h>

#include "PayloadConfig.h"
#include "Storage.h"

/**
 * @brief Implementation of a Storage device to interface with an SD card
 *
 */
class RadioStorage : public Storage {
 private:
  SX1276 radio =
      new Module(RADIO_NSS, RADIO_DIO0, RADIO_RESET, RADIO_DIO1, RADIO_SPI);
  int state;
  uint32_t last_transmission_time;

 public:
  RadioStorage();
  bool verify() override;
  void store(String data) override;
  void storePacket(uint8_t* packet) override;
};

#endif