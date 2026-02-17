#include "drivers/GPSSensor.h"

#include <Wire.h>

#include "SysHead.h"

GPSSensor::GPSSensor() : Sensor("GPS") {}

GPSSensorData GPSSensorData::fromGnss(SFE_UBLOX_GNSS &gnss)
{
  GPSSensorData sensor_data;
  sensor_data.unix_time_s = gnss.getUnixEpoch();
  sensor_data.fix_type = gnss.getFixType();
  sensor_data.fix_ok = gnss.getGnssFixOk();
  sensor_data.siv = gnss.getSIV();
  sensor_data.lat_e7 = gnss.getLatitude();
  sensor_data.lon_e7 = gnss.getLongitude();
  sensor_data.alt_msl_mm = gnss.getAltitudeMSL();
  sensor_data.vel_n_mmps = gnss.getNedNorthVel();
  sensor_data.vel_e_mmps = gnss.getNedEastVel();
  sensor_data.vel_d_mmps = gnss.getNedDownVel();
  sensor_data.hacc_mm = gnss.getHorizontalAccEst();
  sensor_data.vacc_mm = gnss.getVerticalAccEst();

  return sensor_data;
}

bool GPSSensor::verify()
{
  if (gnss.begin() == false)
  {
    return false;
  }

  // Keep I2C output clean and read compact UBX packets.
  gnss.setI2COutput(COM_TYPE_UBX);
  gnss.setAutoPVT(true);

  return true;
}

void GPSSensor::readToSysVar()
{
  if (gnss.getPVT() == false)
  {
    return;
  }

  if (gnss.getInvalidLlh())
  {
    return;
  }

  GPSSensorData sensor_data = GPSSensorData::fromGnss(gnss);

  sysvar_set_gps_data(&sensor_data);
}
