/*****************************************************************
File:        readDataWithIIC.ino
Description: Obtain the temperature,humidity,pressure value through IIC and display the value in the serial port.
******************************************************************/
#include "BME36M280A.h"

BME36M280A myBME280;

float pressure;

void setup()
{
  Serial.begin(9600);
  if(!myBME280.begin()){
    Serial.println("Device error!");
  }
}

void loop()
{
  Serial.print("Temp: ");
  Serial.print(myBME280.readTemperature()); //read and print temperatures
  Serial.println("°C");
  
  Serial.print("Humidity: ");
  Serial.print(myBME280.readHumidity());  //read and print humidity data
  Serial.println("%");

  pressure = myBME280.readPressure(); //read atmospheric pressure data
  Serial.print("Pressure: ");
  Serial.print(pressure);            //print atmospheric pressure data
  Serial.println("Pa");

  Serial.print("Altitude: ");
  Serial.print(myBME280.calcAltitude(pressure)); //read and print altitude data
  Serial.println("m");

  delay(1000);
}
