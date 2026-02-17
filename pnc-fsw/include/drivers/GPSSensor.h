#ifndef GPS_SENSOR_H
#define GPS_SENSOR_H

#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

#include "Sensor.h"

struct GPSSensorData
{
  uint32_t unix_time_s;
  uint8_t fix_type;
  bool fix_ok;
  uint8_t siv;
  int32_t lat_e7;
  int32_t lon_e7;
  int32_t alt_msl_mm;
  int32_t vel_n_mmps;
  int32_t vel_e_mmps;
  int32_t vel_d_mmps;
  uint32_t hacc_mm;
  uint32_t vacc_mm;
};

/**
 * @brief Implementation of a Sensor for the u-blox GNSS receiver
 *
 */
class GPSSensor : public Sensor
{
private:
  SFE_UBLOX_GNSS gnss;

public:
  GPSSensor();
  bool verify() override;
  void readToSysVar() override;
};

#endif
