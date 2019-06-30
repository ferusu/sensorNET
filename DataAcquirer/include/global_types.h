#ifndef global_types_h
#define global_types_h

#include <ESP8266WiFi.h>

typedef enum
{
    GYRO_250 = 0,
    GYRO_500 = 1,
    GYRO_1000 = 2,
    GYRO_2000 = 3
}imuGyroConfig_t;

typedef enum
{
    ACCEL_2 = 0,
    ACCEL_4 = 1,
    ACCEL_8 = 2,
    ACCEL_16 = 3
}imuAccelConfig_t;

typedef struct
{
  /* Start of the packet */
  char id;
  char id2;
  char id3;
  char id4;
  uint32_t timestamp;
  /* Gps frames */
  char ggaFrame[75];
  char rmcFrame[75];
  /* Imu variables */
  int16_t accelX;
  int16_t accelY;
  int16_t accelZ;
  int16_t gyroX;
  int16_t gyroY;
  int16_t gyroZ;
  /* End of string */
  char endOfString;
}packet_t;

typedef struct
{
    char incomingId;
    int16_t chargeLevel;
    int32_t wifiStrenght;
    bool dataCapture;
    imuGyroConfig_t imuGyroConfig;
    imuAccelConfig_t imuAccelConfig;
}orderPacket_t;

#endif