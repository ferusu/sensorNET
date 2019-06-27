/****************************************************************/
/*   Project: SensorNET                                         */
/*   Subproject: DataAquirer                                    */
/*   Author: Ferusu                                             */
/*   Date: 25/02/2019                                           */
/*   Module: main   File: main.cpp                              */
/*   Brief: Main module of the project, contains the main       */
/*   function and setup function.                               */
/****************************************************************/

/*****************************************************************/
/*                    General Includes                           */
/*****************************************************************/

#include <ESP8266WiFi.h>        // Include the Wi-Fi library
#include <WiFiManager.h>
#include <SoftwareSerial.h>     // The serial connection to the GPS module
#include <TinyGPS++.h>
#include <Wire.h>
#include "da_main.h"

/*****************************************************************/
/*                    Project Includes                           */
/*****************************************************************/



/*****************************************************************/
/*                    Object Declaration                         */
/*****************************************************************/

os_timer_t softwareTimer;

/** Software serial object */
SoftwareSerial ss(4,5);

/** Gps object, gives access to the gps functions and data handling */
TinyGPSPlus gps;

/** Udp object, gives access to the Udp functions */
WiFiUDP Udp;

/** IP Address object */
IPAddress remoteIP(192, 168, 1, 200);
IPAddress localIp(192, 168, 1, 201);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

/*****************************************************************/
/*            Typedef of structures and enumerations             */
/*****************************************************************/

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

typedef enum
{
  WAIT_FOR_NMEA_LINE,
  MESSAGE_INIT,
  EVALUATE_HEADER,
  GGA_MESSAGE,
  RMC_MESSAGE,
  OTHER_NMEA_MESSAGE
}nmeaMessage_t;

typedef enum
{
  GGA,
  RMC,
  OTHERS
}headerName_t;

/*****************************************************************/
/*                Private Constant Declaration                   */
/*****************************************************************/

// MPU6050 Slave Device Address
const uint8_t MPU6050SlaveAddress = 0x68;

// Select SDA and SCL pins for I2C communication 
const uint8_t scl = D6;
const uint8_t sda = D7;

// sensitivity scale factor respective to full scale setting provided in datasheet 
const uint16_t AccelScaleFactor = 16384;
const uint16_t GyroScaleFactor = 131;
const int16_t accelXOffset = -418;
const int16_t accelYOffset = 352;
const int16_t accelZOffset = -283;
const int16_t gyroXOffset = 486;
const int16_t gyroYOffset = 18;
const int16_t gyroZOffset = -99;

// MPU6050 few configuration register addresses
const uint8_t MPU6050_REGISTER_SMPLRT_DIV   =  0x19;
const uint8_t MPU6050_REGISTER_USER_CTRL    =  0x6A;
const uint8_t MPU6050_REGISTER_PWR_MGMT_1   =  0x6B;
const uint8_t MPU6050_REGISTER_PWR_MGMT_2   =  0x6C;
const uint8_t MPU6050_REGISTER_CONFIG       =  0x1A;
const uint8_t MPU6050_REGISTER_GYRO_CONFIG  =  0x1B;
const uint8_t MPU6050_REGISTER_ACCEL_CONFIG =  0x1C;
const uint8_t MPU6050_REGISTER_FIFO_EN      =  0x23;
const uint8_t MPU6050_REGISTER_INT_ENABLE   =  0x38;
const uint8_t MPU6050_REGISTER_ACCEL_XOUT_H =  0x3B;
const uint8_t MPU6050_REGISTER_SIGNAL_PATH_RESET  = 0x68;


const char *ssid = "SensorNET";
const char *password = "123456789";
const unsigned int localPort = 2100;      // local port
const unsigned int remotePort = 2000;

const char *gga = "GGA";
const char *rmc = "RMC";

/*****************************************************************/
/*                 Private Variable Declaration                  */
/*****************************************************************/

/* Imu variables */
int16_t Temperature;

/* GPS parser data */
nmeaMessage_t nmeaMessage = WAIT_FOR_NMEA_LINE;
char nmeaPacket[75];

/* Buffer with the imu and gps data */
static packet_t packet;
char packetBuffer[255]; //buffer to hold incoming packet
static uint32_t heartbeatTimestamp = 0;
static bool heartbeatTick = false;
static int heartbeatPeriod = 100;

/*****************************************************************/
/*                  Local Function Prototypes                    */
/*****************************************************************/

void TimerInit (void);
void DataAcquirerInit (void);
void WifiInit (void);
void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data);
void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress);
void ImuOffsetAdjustment (void);
void MPU6050_Init(void);
void ImuInit (void);
void GpsNmeaParser (char character, packet_t *packet_gps);
void GpsNmeaBufferErase (void);
headerName_t GpsNmeaHeaderNameCheck (void);
void GpsHandle (void);
void ImuHandle (void);
void SendBufferUdp (void);

/*****************************************************************/
/*                  Local Function Declaration                   */
/*****************************************************************/

void timerCallback(void *pArg)
{
  heartbeatTimestamp++;
  heartbeatTick = true;
}

void TimerInit (void)
{
  os_timer_setfn(&softwareTimer, timerCallback, NULL);
  os_timer_arm(&softwareTimer, (int)heartbeatPeriod, true);
}

void DataAcquirerInit (void)
{
  packet.id = 0x34;
  packet.id2 = 0x34;
  packet.id3 = 0x34;
  packet.id4 = 0x34;
  packet.accelX = 10;
  packet.accelY = 10;
  packet.accelZ = 10;
  packet.gyroX = 10;
  packet.gyroY = 10;
  packet.gyroZ = 10;
  packet.endOfString = '\0';
}

void WifiInit (void)
{
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.config(localIp, gateway, subnet);
  WiFi.begin(ssid, password);
  pinMode(D4, OUTPUT);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(200);
    digitalWrite(D4, !digitalRead(D4));
  }
  digitalWrite(D4, HIGH);
  Serial.print("Conectando a:\t");
  Serial.println(ssid);
  Udp.begin(localPort);
}

void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data)
{
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.write(data);
  Wire.endTransmission();
}

void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress)
{
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t)14);
  packet.accelX = (((int16_t)Wire.read()<<8) | Wire.read());
  packet.accelY = (((int16_t)Wire.read()<<8) | Wire.read());
  packet.accelZ = (((int16_t)Wire.read()<<8) | Wire.read());
  Temperature = (((int16_t)Wire.read()<<8) | Wire.read());
  packet.gyroX = (((int16_t)Wire.read()<<8) | Wire.read());
  packet.gyroY = (((int16_t)Wire.read()<<8) | Wire.read());
  packet.gyroZ = (((int16_t)Wire.read()<<8) | Wire.read());
}

void ImuOffsetAdjustment (void)
{
  packet.accelX = packet.accelX + accelXOffset;
  packet.accelY = packet.accelY + accelYOffset;
  packet.accelZ = packet.accelZ + accelZOffset;
  packet.gyroX = packet.gyroX + gyroXOffset;
  packet.gyroY = packet.gyroY + gyroYOffset;
  packet.gyroZ = packet.gyroZ + gyroZOffset;
}

void MPU6050_Init(void)
{
  delay(150);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SMPLRT_DIV, 0x07);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_1, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_PWR_MGMT_2, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_CONFIG, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_GYRO_CONFIG, 0x00);//set +/-250 degree/second full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_CONFIG, 0x00);// set +/- 2g full scale
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_FIFO_EN, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_INT_ENABLE, 0x01);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_SIGNAL_PATH_RESET, 0x00);
  I2C_Write(MPU6050SlaveAddress, MPU6050_REGISTER_USER_CTRL, 0x00);
}

void ImuInit (void)
{
  Wire.begin(sda, scl);
  MPU6050_Init();
}

void GpsNmeaParser (char character, packet_t *packet_gps)
{
  static int nmeaIndex;
  static int headerCounter;
  switch(nmeaMessage)
  {
    case WAIT_FOR_NMEA_LINE:
      if(character == '\n')
      {
        nmeaMessage = MESSAGE_INIT;
      }
      break;

    case MESSAGE_INIT:
      GpsNmeaBufferErase();
      nmeaIndex=0;
      nmeaPacket[nmeaIndex]=character;
      nmeaMessage = EVALUATE_HEADER;
      break;

    case EVALUATE_HEADER:
      nmeaIndex++;
      nmeaPacket[nmeaIndex]=character;
      headerCounter++;
      if(headerCounter>=5)
      {
        headerCounter = 0;
        switch (GpsNmeaHeaderNameCheck())
        {
          case GGA:
          nmeaMessage = GGA_MESSAGE;
          break;
          case RMC:
          nmeaMessage = RMC_MESSAGE;
          break;
          case OTHERS:
          nmeaMessage = OTHER_NMEA_MESSAGE;
        }
      }
      break;

    case GGA_MESSAGE:
      if (character == '\n')
      {
        nmeaMessage = MESSAGE_INIT;
        memcpy(packet_gps->ggaFrame, nmeaPacket, sizeof(nmeaPacket));
        //Serial.println(packet_gps->ggaFrame);
      }
      else
      {
        nmeaIndex++;
        nmeaPacket[nmeaIndex]=character;
      }
      break;

    case RMC_MESSAGE:
      if (character == '\n')
      {
        nmeaMessage = MESSAGE_INIT;
        memcpy(packet_gps->rmcFrame, nmeaPacket, sizeof(nmeaPacket));
        //Serial.println(packet_gps->rmcFrame);
      }
      else
      {
        nmeaIndex++;
        nmeaPacket[nmeaIndex]=character;
      }
      break;

    case OTHER_NMEA_MESSAGE:
      if (character == '\n')
      {
        nmeaMessage = MESSAGE_INIT;
      }
      else
      {
        nmeaIndex++;
        nmeaPacket[nmeaIndex]=character;
      }
      break;

    default:
      nmeaMessage = WAIT_FOR_NMEA_LINE;
      break;
  }
}

void GpsNmeaBufferErase (void)
{
  int nmeaIndex;
  for (nmeaIndex=0; nmeaIndex<75; nmeaIndex++)
  {
    nmeaPacket[nmeaIndex]=0;
  }
}

headerName_t GpsNmeaHeaderNameCheck (void)
{
  headerName_t headerName;
  bool result = true;
  int nmeaIndex;
  for (nmeaIndex=0; nmeaIndex<3; nmeaIndex++)
  {
    result &= (nmeaPacket[nmeaIndex+3] == gga[nmeaIndex]);
  }
  if (result==true)
  {
    headerName = GGA;
  }
  else
  {
    result=true;
    for (nmeaIndex=0; nmeaIndex<3; nmeaIndex++)
    {
      result &= (nmeaPacket[nmeaIndex+3] == rmc[nmeaIndex]);
    }
    if (result==true)
    {
      headerName = RMC;
    }
    else
    {
      headerName = OTHERS;
    }
    
  }
  return headerName;
}

void GpsHandle (void)
{
  while (ss.available() > 0)
  {
    GpsNmeaParser(ss.read(), &packet);
  }
}

void ImuHandle (void)
{
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
  ImuOffsetAdjustment();
}

void SendBufferUdp (void)
{
  memcpy(packetBuffer, &packet, sizeof(packet));
  //Serial.print(packet.ggaFrame);
  //Serial.println(";");
  //Serial.print(packet.rmcFrame);
  //Serial.println(";");
  Udp.beginPacket(remoteIP, remotePort);
  Udp.write(packetBuffer,sizeof(packet));
  Udp.endPacket();
}

/*****************************************************************/
/*                  Public Function Declaration                  */
/*****************************************************************/

void setup()
{
  Serial.begin(9600);
  ss.begin(9600);
  TimerInit();
  DataAcquirerInit ();
  ImuInit();
  WifiInit();
}

void loop()
{
  if(heartbeatTick)
  {
    heartbeatTick = false;
    packet.timestamp = heartbeatTimestamp;
    ImuHandle();
    SendBufferUdp();
  }
  GpsHandle();
}

