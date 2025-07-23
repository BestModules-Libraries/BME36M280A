/*****************************************************************
File:             BME36M280A.h
Author:           BEST MODULES CORP.
Description:      Define classes and required variables
Version:          V1.0.1   -- 2025-04-16
******************************************************************/
#ifndef _BME36M280A_H_
#define _BME36M280A_H_

#include <Arduino.h>
#include <Wire.h>

#define BME280_ADDRESS   0x77

#define BME280_REG_DIG_T1    0x88
#define BME280_REG_DIG_T2    0x8A
#define BME280_REG_DIG_T3    0x8C

#define BME280_REG_DIG_P1    0x8E
#define BME280_REG_DIG_P2    0x90
#define BME280_REG_DIG_P3    0x92
#define BME280_REG_DIG_P4    0x94
#define BME280_REG_DIG_P5    0x96
#define BME280_REG_DIG_P6    0x98
#define BME280_REG_DIG_P7    0x9A
#define BME280_REG_DIG_P8    0x9C
#define BME280_REG_DIG_P9    0x9E

#define BME280_REG_DIG_H1    0xA1
#define BME280_REG_DIG_H2    0xE1
#define BME280_REG_DIG_H3    0xE3
#define BME280_REG_DIG_H4    0xE4
#define BME280_REG_DIG_H5    0xE5
#define BME280_REG_DIG_H6    0xE7

#define BME280_REG_CHIPID          0xD0
#define BME280_REG_VERSION         0xD1
#define BME280_REG_SOFTRESET       0xE0

#define BME280_REG_CAL26           0xE1

#define BME280_REG_CONTROLHUMID    0xF2
#define BME280_REG_CONTROL         0xF4
#define BME280_REG_CONFIG          0xF5
#define BME280_REG_PRESSUREDATA    0xF7
#define BME280_REG_TEMPDATA        0xFA
#define BME280_REG_HUMIDITYDATA    0xFD


class BME36M280A
{
public:
  BME36M280A(TwoWire *theWire = &Wire);
  bool begin(uint8_t  i2c_addr = BME280_ADDRESS);
  float readTemperature(bool isFahrenheit = false);
  uint32_t readHumidity(void);
  uint32_t readPressure(void);
  float calcAltitude(float pressure);
private: 
  TwoWire *_theWire;
  uint8_t _deviceAddress;
  // Calibratino data
  uint16_t dig_T1;
  int16_t dig_T2;
  int16_t dig_T3;

  uint16_t dig_P1;
  int16_t dig_P2;
  int16_t dig_P3;
  int16_t dig_P4;
  int16_t dig_P5;
  int16_t dig_P6;
  int16_t dig_P7;
  int16_t dig_P8;
  int16_t dig_P9;

  uint8_t dig_H1;
  int16_t dig_H2;
  uint8_t dig_H3;
  int16_t dig_H4;
  int16_t dig_H5;
  int8_t  dig_H6;

  int32_t t_fine;

  uint8_t BME280Read8(uint8_t reg);
  uint16_t BME280Read16(uint8_t reg);
  uint16_t BME280Read16LE(uint8_t reg);
  int16_t BME280ReadS16(uint8_t reg);
  int16_t BME280ReadS16LE(uint8_t reg);
  uint32_t BME280Read24(uint8_t reg);
  void writeRegister(uint8_t reg, uint8_t val);
};

#endif
