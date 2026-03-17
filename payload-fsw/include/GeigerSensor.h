#ifndef GEIGER_SENSOR_H
#define GEIGER_SENSOR_H

#include "Sensor.h"
#include "GeigerCounter.h"
#include "PayloadConfig.h"

class GeigerSensor : public Sensor {
  private: 
    GeigerCounter gc; 

  public: 
    GeigerSensor(); 
    GeigerSensor(unsigned long minimum_period); 
    bool verify() override; 
    String readData() override; 
    void readDataPacket(uint8_t*& packet) override; 
    String decodeToCSV(uint8_t*& packet) override; 
};

#endif 