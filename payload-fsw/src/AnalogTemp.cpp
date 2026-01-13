#include "AnalogTemp.h"

#include "PayloadConfig.h"

/**
 * @brief Construct a new Analog Temp object with minimum period of 1000 ms
 *
 */
AnalogTemp::AnalogTemp() : AnalogTemp(1000) {}

/**
 * @brief Construct a new Analog Temp object with given minimum period
 *
 * @param minimum_period Minimum period between readings in ms
 */
AnalogTemp::AnalogTemp(unsigned long minimum_period)
    : Sensor("AnalogTemp", "ADC_Read,", minimum_period) {}

/**
 * @brief Set up sensor and returns status (always true for the thermistor)
 *
 * @return true
 * @return false
 */
bool AnalogTemp::verify() {
  pinMode(THERMISTOR_PIN, INPUT);
  return true;
}

/**
 * @brief Reads ADC value, returns String
 *
 * @return String ADC value
 */
String AnalogTemp::readData() { return String(analogRead(THERMISTOR_PIN)); }

/**
 * @brief Reads ADC value and appends to packet
 *
 * @param packet Pointer to the packet byte array
 */
void AnalogTemp::readDataPacket(uint8_t*& packet) {
  int adc_reading = analogRead(THERMISTOR_PIN);

  memcpy(packet, &adc_reading, sizeof(int));

  packet += sizeof(int);
}

/**
 * @brief Decodes ADC reading from packet array
 *
 * @param packet Pointer to the packet byte array
 * @return String ADC reading
 */
String AnalogTemp::decodeToCSV(uint8_t*& packet) {
  int adc_reading = 0;

  memcpy(&adc_reading, packet, sizeof(int));

  packet += sizeof(int);

  return String(adc_reading);
}