/*!
 * @file HTU21D.h
 */

#ifndef _HTU21D_H
#define _HTU21D_H

#include "Arduino.h"
#include "Wire.h"

/**
 * HTU21D Measurement Resolution
 */
enum HTU21DResolution {
  RESOLUTION_RH12_T14 = 0, //!< 12 bit for RH and 14 bit for temperature
  RESOLUTION_RH8_T12 = 1,  //!< 8 bit for RH and 12 bit for temperature
  RESOLUTION_RH10_T13 = 2, //!< 10 bit for RH and 13 bit for temperature
  RESOLUTION_RH11_T11 = 3  //!< 11 bit for RH and 11 bit for temperature
};

/**
 * HTU21D Sensor Driver
 */
class HTU21D {
private:
  static const int HTU21D_ADDR = 0x40;

  const int _addr;
  TwoWire& _wire;
  HTU21DResolution _resolution;
  
  float temperature;
  float humidity;
  
  enum HTU21DCmd {
    TRIGGER_TEMP_MEAS_H = 0xE3,
    TRIGGER_HUM_MEAS_H = 0xE5,
    TRIGGER_TEMP_MEAS_NH = 0xF3,
    TRIGGER_HUM_MEAS_NH = 0xF5,
    WRITE_USER_REG = 0xE6,
    READ_USER_REG = 0xE7,
    SOFT_RESET = 0xFE
  };
  
  bool measureTemperature();
  bool measureHumidity();
  bool checkCRC8(uint8_t data[]);
public:
  HTU21D(int addr = HTU21D_ADDR, TwoWire& wire = Wire);
  
  bool measure();
  float getTemperature(void) const;
  float getHumidity(void) const;
  void setResolution(HTU21DResolution resolution);
  HTU21DResolution getResolution(void);
  bool reset(void);
  bool begin(void);
};


#endif
