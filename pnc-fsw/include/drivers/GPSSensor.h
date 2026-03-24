#ifndef GPS_SENSOR_H
#define GPS_SENSOR_H

#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

#include "Sensor.h"

struct __attribute__((packed)) GPSSensorData {
  uint32_t unix_time_s;
  uint8_t fix_type;
  bool fix_ok;
  uint8_t siv;
  uint8_t buff = 0;  // additional byte to avoid word misalignment in packed
  int32_t lat_e7;
  int32_t lon_e7;
  int32_t alt_msl_mm;
  int32_t vel_n_mmps;
  int32_t vel_e_mmps;
  int32_t vel_d_mmps;
  int32_t hacc_mm;
  int32_t vacc_mm;

  static GPSSensorData fromGnss(SFE_UBLOX_GNSS& gnss);
};

/**
 * @brief Implementation of a Sensor for the u-blox GNSS receiver
 *
 */
class GPSSensor : public Sensor {
 private:
  SFE_UBLOX_GNSS gnss;

 public:
  GPSSensor();
  bool verify() override;
  void readToSysVar() override;
};

#endif
