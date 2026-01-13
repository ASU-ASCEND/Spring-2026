#ifndef ANALOGTEMP_H
#define ANALOGTEMP_H

#include "Sensor.h"

class AnalogTemp : public Sensor {
 private:
 public:
  AnalogTemp();
  AnalogTemp(unsigned long minimum_period);

  bool verify() override;
  String readData() override;
  void readDataPacket(uint8_t*& packet) override;
  String decodeToCSV(uint8_t*& packet) override;
};

#endif