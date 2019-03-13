
#ifndef daman_imu_h
#define daman_imu_h
 
#include <ESP8266WiFi.h>

typedef struct
{
  int16_t accelX;
  int16_t accelY;
  int16_t accelZ;
  int16_t temperature;
  int16_t gyroX;
  int16_t gyroY;
  int16_t gyroZ;
}imuData_t;

 
#endif