#ifndef daapp_stat_h
#define daapp_stat_h

#include <ESP8266WiFi.h>

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

#endif