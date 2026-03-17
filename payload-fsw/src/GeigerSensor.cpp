#include "GeigerSensor.h"

GeigerSensor::GeigerSensor() : GeigerSensor(1000) {

}

GeigerSensor::GeigerSensor(unsigned long minimum_period) : 
  Sensor("GeigerCounter", "CPS, Dose: ", minimum_period) {
}

bool GeigerSensor::verify() {
  this->gc.begin(GEIGER_PIN); // not using enforced minimums 

  return true; 
}

String GeigerSensor::readData(){
  return String(this->gc.getInstCPSRunning()) + "," + String(this->gc.getInstDoseRunning()) + ","; 
} 

void GeigerSensor::readDataPacket(uint8_t*& packet){
  float cps = this->gc.getInstCPSRunning(); 
  float dose = this->gc.getInstDoseRunning(); 

  memcpy(packet, &cps, sizeof(float)); 
  packet += sizeof(float); 
  memcpy(packet, &dose, sizeof(float)); 
  packet += sizeof(float); 
} 

String GeigerSensor::decodeToCSV(uint8_t*& packet){
  float cps, dose; 
  memcpy(&cps, packet, sizeof(float)); 
  packet += sizeof(float); 
  memcpy(&dose, packet, sizeof(float)); 
  packet += sizeof(float); 
  return String(cps) + "," + String(dose) + ","; 
}