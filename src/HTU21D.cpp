/*!
 * @file HTU21D.cpp
 * @mainpage HTU21D Sensor Library
 *
 * This is an Arduino library for the HTU21D temperature and humidity sensor.
 * 
 * For details on the sensor please refer to the datasheet.
 * It can be found on the manufactures page:
 * https://www.te.com/deu-de/product-CAT-HSC0004.html
 *
 * This library was written by Daniel Wiese (DevXplained).
 * 
 * Redistribution is possible under the terms of the MIT license.
 */

#include "HTU21D.h"

#ifdef __AVR__
#include <avr/pgmspace.h>
#endif

#ifdef __AVR__
static const uint8_t HTU21D_DELAY_T[] PROGMEM = {50, 13, 25, 7};
static const uint8_t HTU21D_DELAY_H[] PROGMEM = {16, 3, 5, 8};
#else
static const uint8_t HTU21D_DELAY_T[] = {50, 13, 25, 7};
static const uint8_t HTU21D_DELAY_H[] = {16, 3, 5, 8};
#endif

static const float HTU21D_TCoeff = -0.15;

/**
 * Constructor
 * Initializes a new sensor instance
 * 
 * @param addr Sensor Address (default 0x40)
 * @param wire TWI bus instance (default Wire)
 */
HTU21D::HTU21D(uint8_t addr, TwoWire& wire) : _addr(addr), _wire(wire), _resolution(RESOLUTION_RH12_T14) {
  
}

bool HTU21D::checkCRC8(uint8_t data[]) {
  uint8_t crc = 0;
  for(int i = 0; i < 2; i++) {
    crc ^= data[i];
    for(int b = 7; b >= 0; b--) {
      if(crc & 0x80) crc = (crc << 1) ^ 0x31;
      else crc = (crc << 1);
    }
  }
  
  return crc == data[2];
}

bool HTU21D::measureTemperature() {
  /* Measure temperature */
  _wire.beginTransmission(_addr);
  _wire.write(TRIGGER_TEMP_MEAS_NH);
  _wire.endTransmission(false); 
  
#ifdef __AVR__
  delay(pgm_read_byte_near(HTU21D_DELAY_T + _resolution));
#else
  delay(HTU21D_DELAY_T[_resolution]);
#endif
  
  _wire.requestFrom(_addr, 3);
  if(_wire.available() != 3) return false;
  
  uint8_t data[3];
  for(int i = 0; i < 3; i++) data[i] = _wire.read();
  if(!checkCRC8(data)) return false;
  
  uint16_t St = (data[0] << 8) | (data[1] & 0xFC);
  temperature = -46.85 + 175.72 * St / 65536.0;
  
  return true;
}

bool HTU21D::measureHumidity() {
  /* Measure humidity */
  _wire.beginTransmission(_addr);
  _wire.write(TRIGGER_HUM_MEAS_NH);
  _wire.endTransmission(false); 
  
#ifdef __AVR__
  delay(pgm_read_byte_near(HTU21D_DELAY_H + _resolution));
#else
  delay(HTU21D_DELAY_H[_resolution]);
#endif
  
  _wire.requestFrom(_addr, 3);
  if(_wire.available() != 3) return false;
  
  uint8_t data[3];
  for(uint8_t i = 0; i < 3; i++) data[i] = _wire.read();
  if(!checkCRC8(data)) return false;
  
  uint16_t Srh = (data[0] << 8) | (data[1] & 0xFC);
  humidity = -6.0 + 125.0 * Srh / 65536.0;
  humidity += (25.0 - temperature) * HTU21D_TCoeff;
  humidity = constrain(humidity, 0.0, 100.0);
  
  return true;
}

/**
 * Starts a temperature and a humidity measurement and reads the result.
 * @return true if the result was read correctly, otherwise false
 */
bool HTU21D::measure() {
  /* Reset values */
  temperature = NAN;
  humidity = NAN;
  
  /* NOTE: Order is important as the temperature is needed to correct the humidity reading */
  if(!measureTemperature()) return false;
  if(!measureHumidity()) return false;
  return true;
}

/**
 * Sets the measurement resolution.
 * 
 * @param resolution Resolution 
 * @see HTU21DResolution for possible resolutions
 */
void HTU21D::setResolution(HTU21DResolution resolution) {
  _wire.beginTransmission(_addr);
  _wire.write(WRITE_USER_REG);
  _wire.write((resolution & 0x01) | ((resolution & 0x02) << 6) | 0x02);
  _wire.endTransmission(); 
  
  _resolution = resolution;
}

/**
 * Get the current resolution
 * @return Resolution
 * @see HTU21DResolution for possible resolutions
 */
HTU21DResolution HTU21D::getResolution() {
  return _resolution;
}

/**
 * Initializes the I2C transport (Wire.begin()) and resets the sensor.
 * @return true if the initialization was successful, otherwise false
 */
bool HTU21D::begin() {
  _wire.begin();
  return reset();
}
  
/**
 * Resets the sensor to its default settings and resolution.
 * @return true if the reset was successful, otherwise false
 */
bool HTU21D::reset() {
  _wire.beginTransmission(_addr);
  _wire.write(SOFT_RESET);
  _wire.endTransmission(); 
  
  delay(15);
  
  _wire.beginTransmission(_addr);
  _wire.write(READ_USER_REG);
  _wire.endTransmission(false); 
  _wire.requestFrom(_addr, 1);
  if(_wire.available() != 1) return false;
  if(_wire.read() != 0x02) return false;
  
  _resolution = RESOLUTION_RH12_T14;

  return true;
}

/**
 * Returns the temperature value acquired with the last measurement.
 * To refresh this value call measure().
 * 
 * @returns Temperature in °C or NaN
 */
float HTU21D::getTemperature() const {
  return temperature;
}

/**
 * Returns the humidity value acquired with the last measurement.
 * To refresh this value call measure().
 * 
 * @returns Relative Humidity in percent or NaN
 */
float HTU21D::getHumidity() const {
  return humidity;
}
