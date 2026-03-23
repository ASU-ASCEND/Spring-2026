#include "GeigerSensor.h"

GeigerSensor::GeigerSensor() : GeigerSensor(1000) {}

GeigerSensor::GeigerSensor(unsigned long minimum_period)
    : Sensor("GeigerCounter", "CPS,Dose,", minimum_period) {}

bool GeigerSensor::verify() {
  this->gc.begin(GEIGER_PIN, 1000);  // using enforced minimum of 1000 ms

  return true;
}

String GeigerSensor::readData() {
  return String(this->gc.getCPSRunning()) + "," +
         String(this->gc.getDoseRunning()) + ",";
}

void GeigerSensor::readDataPacket(uint8_t*& packet) {
  float cps = this->gc.getCPSRunning();
  float dose = this->gc.getDoseRunning();

  memcpy(packet, &cps, sizeof(float));
  packet += sizeof(float);
  memcpy(packet, &dose, sizeof(float));
  packet += sizeof(float);
}

String GeigerSensor::decodeToCSV(uint8_t*& packet) {
  float cps, dose;
  memcpy(&cps, packet, sizeof(float));
  packet += sizeof(float);
  memcpy(&dose, packet, sizeof(float));
  packet += sizeof(float);
  return String(cps) + "," + String(dose) + ",";
}