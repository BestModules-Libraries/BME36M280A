/*****************************************************************
File:             BME36M280A.cpp
Author:           BEST MODULES CORP.
Description:      The sensor with IIC and obtain the corresponding value
Version:          V1.0.1   -- 2025-04-16
******************************************************************/
#include "BME36M280A.h"

/**********************************************************
Description: Constructor
Parameters:  *twotheWire: &Wire or &Wire1 or &Wire2 on BMduino UNO
Return:  None
Others:  None    
**********************************************************/
BME36M280A::BME36M280A(TwoWire *theWire)
{
  _theWire = theWire;
}

/**********************************************************
Description: Module initial
Parameters: i2c_addr: Device Address,0x76 or 0x77(default)
Return:    Module initialization status:true/false
Others:    None   
**********************************************************/
bool BME36M280A::begin(uint8_t  i2c_addr)
{
  _deviceAddress = i2c_addr;
  _theWire->begin();
  delay(2);

  if(BME280Read8(BME280_REG_CHIPID) != 0x60)
    return false;

  dig_T1 = BME280Read16LE(BME280_REG_DIG_T1);
  dig_T2 = BME280ReadS16LE(BME280_REG_DIG_T2);
  dig_T3 = BME280ReadS16LE(BME280_REG_DIG_T3);

  dig_P1 = BME280Read16LE(BME280_REG_DIG_P1);
  dig_P2 = BME280ReadS16LE(BME280_REG_DIG_P2);
  dig_P3 = BME280ReadS16LE(BME280_REG_DIG_P3);
  dig_P4 = BME280ReadS16LE(BME280_REG_DIG_P4);
  dig_P5 = BME280ReadS16LE(BME280_REG_DIG_P5);
  dig_P6 = BME280ReadS16LE(BME280_REG_DIG_P6);
  dig_P7 = BME280ReadS16LE(BME280_REG_DIG_P7);
  dig_P8 = BME280ReadS16LE(BME280_REG_DIG_P8);
  dig_P9 = BME280ReadS16LE(BME280_REG_DIG_P9);

  dig_H1 = BME280Read8(BME280_REG_DIG_H1);
  dig_H2 = BME280Read16LE(BME280_REG_DIG_H2);
  dig_H3 = BME280Read8(BME280_REG_DIG_H3);
  dig_H4 = (BME280Read8(BME280_REG_DIG_H4) << 4) | (0x0F & BME280Read8(BME280_REG_DIG_H4 + 1));
  dig_H5 = (BME280Read8(BME280_REG_DIG_H5 + 1) << 4) | (0x0F & BME280Read8(BME280_REG_DIG_H5) >> 4);
  dig_H6 = (int8_t)BME280Read8(BME280_REG_DIG_H6);

  writeRegister(BME280_REG_CONTROLHUMID, 0x05);  //Choose 16X oversampling
  writeRegister(BME280_REG_CONTROL, 0xB7);  //Choose 16X oversampling

  return true;
}

/**********************************************************
Description: readTemperature
Parameters:  isFahrenheit = true:Fahrenheit degree
             isFahrenheit = false:centigrade(default)
Return:      temperature , unit:℃（default） or  ℉ 
Others:      
**********************************************************/
float BME36M280A::readTemperature(bool isFahrenheit)
{
  int32_t var1, var2;

  int32_t adc_T = BME280Read24(BME280_REG_TEMPDATA);
  adc_T >>= 4;
  var1 = (((adc_T >> 3) - ((int32_t)(dig_T1 << 1))) *
    ((int32_t)dig_T2)) >> 11;

  var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) *
    ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) *
    ((int32_t)dig_T3)) >> 14;

  t_fine = var1 + var2;
  float T = (t_fine * 5 + 128) >> 8;

  if(isFahrenheit == true)
  {
    return T*1.8 + 32;
  }
  else
  {
    return T/100;
  }
}

/**********************************************************
Description: readHumidity
Parameters:  void
Return:      humidity,uint:%RH
Others:      None
**********************************************************/
uint32_t BME36M280A::readHumidity(void)
{
  int32_t v_x1_u32r, adc_H;

  // Call getTemperature to get t_fine
  readTemperature();

  adc_H = BME280Read16(BME280_REG_HUMIDITYDATA);

  v_x1_u32r = (t_fine - ((int32_t)76800));
  v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10) * (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)dig_H2) + 8192) >> 14));
  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));
  v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
  v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
  return (uint32_t)(v_x1_u32r>>12)/1024.0;
}

/**********************************************************
Description: readPressure
Parameters:  void
Return:      pressure,uint:Pa
Others:      None
**********************************************************/
uint32_t BME36M280A::readPressure(void)
{
  int64_t var1, var2, p;

  // Call getTemperature to get t_fine
  readTemperature();

  int32_t adc_P = BME280Read24(BME280_REG_PRESSUREDATA);
  adc_P >>= 4;

  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)dig_P6;
  var2 = var2 + ((var1*(int64_t)dig_P5)<<17);
  var2 = var2 + (((int64_t)dig_P4)<<35);
  var1 = ((var1 * var1 * (int64_t)dig_P3)>>8) + ((var1 * (int64_t)dig_P2)<<12);
  var1 = (((((int64_t)1)<<47)+var1))*((int64_t)dig_P1)>>33;
  if (var1 == 0)
  {
  return 0; // avoid exception caused by division by zero
  }
  p = 1048576-adc_P;
  p = (((p<<31)-var2)*3125)/var1;
  var1 = (((int64_t)dig_P9) * (p>>13) * (p>>13)) >> 25;
  var2 = (((int64_t)dig_P8) * p) >> 19;
  p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7)<<4);
  return (uint32_t)p/256;
}

/**********************************************************
Description: calcAltitude
Parameters:  pressure:pressure value,uint Pa
Return:      Altitude,uint:m
Others:      None
**********************************************************/
float BME36M280A::calcAltitude(float pressure)
{
  float A = pressure/101325;
  float B = 1/5.25588;
  float C = pow(A,B);
  C = 1.0 - C;
  C = C /0.0000225577;
  return C;
}

/**********************************************************
Description: BME280Read8
Parameters:  reg:Register address
Return:      Register Value
Others:      None
**********************************************************/
uint8_t BME36M280A::BME280Read8(uint8_t reg)
{
  _theWire->beginTransmission(_deviceAddress);
  _theWire->write(reg);
  _theWire->endTransmission();

  _theWire->requestFrom(_deviceAddress, 1);
  while(!_theWire->available());
  return _theWire->read();
}
/**********************************************************
Description: BME280Read16
Parameters:  reg:Register address
Return:      Register Value
Others:      None
**********************************************************/
uint16_t BME36M280A::BME280Read16(uint8_t reg)
{
  uint8_t msb, lsb;

  _theWire->beginTransmission(_deviceAddress);
  _theWire->write(reg);
  _theWire->endTransmission();

  _theWire->requestFrom(_deviceAddress, 2);
  while(_theWire->available()<2);
  msb = _theWire->read();
  lsb = _theWire->read();

  return (uint16_t) msb<<8 | lsb;
}
/**********************************************************
Description: BME280Read16LE
Parameters:  reg:Register address
Return:      Register Value
Others:      None
**********************************************************/
uint16_t BME36M280A::BME280Read16LE(uint8_t reg)
{
  uint16_t data = BME280Read16(reg);
  return (data >> 8) | (data << 8);
}
/**********************************************************
Description: BME280ReadS16
Parameters:  reg:Register address
Return:      Register Value
Others:      None
**********************************************************/
int16_t BME36M280A::BME280ReadS16(uint8_t reg)
{
  return (int16_t)BME280Read16(reg);
}
/**********************************************************
Description: BME280ReadS16LE
Parameters:  reg:Register address
Return:      Register Value
Others:      None
**********************************************************/
int16_t BME36M280A::BME280ReadS16LE(uint8_t reg)
{
  return (int16_t)BME280Read16LE(reg);
}
/**********************************************************
Description: BME280Read24
Parameters:  reg:Register address
Return:      Register Value
Others:      None
**********************************************************/
uint32_t BME36M280A::BME280Read24(uint8_t reg)
{
  uint32_t data;

  _theWire->beginTransmission(_deviceAddress);
  _theWire->write(reg);
  _theWire->endTransmission();

  _theWire->requestFrom(_deviceAddress, 3);
  while(_theWire->available()<3);
  data = _theWire->read();
  data <<= 8;
  data |= _theWire->read();
  data <<= 8;
  data |= _theWire->read();

  return data;
}
/**********************************************************
Description: writeRegister
Parameters:  reg:Register address
             val：data
Return:      void
Others:      None
**********************************************************/
void BME36M280A::writeRegister(uint8_t reg, uint8_t val)
{
  _theWire->beginTransmission(_deviceAddress); // start transmission to device
  _theWire->write(reg);       // send register address
  _theWire->write(val);         // send value to write
  _theWire->endTransmission();     // end transmission
}
