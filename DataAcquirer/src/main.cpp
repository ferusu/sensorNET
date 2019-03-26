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
  uint32_t timestamp;
  /* Gps variables */
  float latitudeDeg;
  float longitudeDeg;
  double courseDeg;
  double speedKmph;
  uint8_t timeHour;
  uint8_t timeMinute;
  uint8_t timeSecond;
  uint8_t timeCentisecond;
  uint8_t numberOfSatellites;
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

/*****************************************************************/
/*                 Private Variable Declaration                  */
/*****************************************************************/

/* Gps variables */
static float latitude , longitude;
static double course, kmph;
static uint8_t hour, minute, second, centisecond, satellites;

/* Imu variables */
int16_t AccelX, AccelY, AccelZ, Temperature, GyroX, GyroY, GyroZ;

/* Buffer with the imu and gps data */
static packet_t packet;
unsigned int localPort = 2100;      // local port
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
void MPU6050_Init(void);
void ImuInit (void);
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
  packet.endOfString = '\0';
  packet.id = 0x55;
}

void WifiInit (void)
{

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

void GpsHandle (void)
{
  while (ss.available() > 0)
  {
    if (gps.encode(ss.read()))
    {
      if (gps.location.isValid())
      {
        packet.latitudeDeg = gps.location.lat();
        packet.longitudeDeg = gps.location.lng();
      }
      if (gps.time.isValid())
      {
        packet.timeHour = gps.time.hour();
        packet.timeMinute = gps.time.minute();
        packet.timeSecond = gps.time.second();
        packet.timeCentisecond = gps.time.centisecond();
      }
      if (gps.satellites.isValid())
      {
        packet.numberOfSatellites = (uint8_t)gps.satellites.value();
      }
      if (gps.course.isValid())
      {
        packet.courseDeg = gps.course.deg();
      }
      if (gps.speed.isValid())
      {
        packet.speedKmph = gps.speed.kmph();
      }
    }
  }
}

void ImuHandle (void)
{
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
}

void SendBufferUdp (void)
{
  memcpy(packetBuffer, &packet, sizeof(packet));
  Udp.beginPacket(remoteIP, 2000);
  Udp.write(packetBuffer);
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

