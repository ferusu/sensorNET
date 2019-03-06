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
/*            Typedef of structures and enumerations             */
/*****************************************************************/

typedef struct
{
  int16_t accelX;
  int16_t accelY;
  int16_t accelZ;
  int16_t gyroX;
  int16_t gyroY;
  int16_t gyroZ;
}imuData_t;

typedef struct
{
  float latitude;
  float longitude;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
  uint8_t centisecond;
}gpsData_t;

/*****************************************************************/
/*                    Object Declaration                         */
/*****************************************************************/

/** Software serial object */
SoftwareSerial ss(4,5);

/** Gps object, gives access to the gps functions and data handling */
TinyGPSPlus gps;

/** Udp object, gives access to the Udp functions */
WiFiUDP Udp;

/** IP Address object */
IPAddress remoteIP(192, 168, 0, 14);
IPAddress localIp(192, 168, 0, 201);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

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


const char *ssid = "Budhahouse";
const char *password = "corbalmasid";

/*****************************************************************/
/*                 Private Variable Declaration                  */
/*****************************************************************/

/* Gps variables */
//float latitude , longitude;
gpsData_t gpsData;

/* Imu variables */
//int16_t AccelX, AccelY, AccelZ, Temperature, GyroX, GyroY, GyroZ;
int16_t Temperature;
imuData_t imuData;
uint8_t id = 0;

/* Printable variables */

unsigned int localPort = 2100;      // local port to listen on
char packetBuffer[24]; //buffer to hold incoming packet

/*****************************************************************/
/*                  Local Function Prototypes                    */
/*****************************************************************/

void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data);
void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress);
void MPU6050_Init(void);
void GpsHandle (void);
void ImuHandle (void);
void ParseImuData (void);
void ParseGpsData (void);
void SendBufferUdp (void);
void PrintVariables (void);

/*****************************************************************/
/*                  Local Function Declaration                   */
/*****************************************************************/

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
  imuData.accelX = (((int16_t)Wire.read()<<8) | Wire.read());
  imuData.accelY = (((int16_t)Wire.read()<<8) | Wire.read());
  imuData.accelZ = (((int16_t)Wire.read()<<8) | Wire.read());
  Temperature = (((int16_t)Wire.read()<<8) | Wire.read());
  imuData.gyroX = (((int16_t)Wire.read()<<8) | Wire.read());
  imuData.gyroX = (((int16_t)Wire.read()<<8) | Wire.read());
  imuData.gyroX = (((int16_t)Wire.read()<<8) | Wire.read());
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

void GpsHandle (void)
{
  while (ss.available() > 0)
  {
    if (gps.encode(ss.read()))
    {
      if (gps.location.isValid())
      {
        gpsData.latitude = gps.location.lat();
        //Serial.print("Latitud:");
        //Serial.println(String(latitude , 6));
        gpsData.longitude = gps.location.lng();
        //Serial.print("Longitud:");
        //Serial.println(String(longitude , 6));
      }
      if (gps.time.isValid())
      {
        gpsData.hour = gps.time.hour();
        gpsData.minute = gps.time.minute();
        gpsData.second = gps.time.second();
        gpsData.centisecond = gps.time.centisecond();
      }
    }
  }
}

void ImuHandle (void)
{
  //double Ax, Ay, Az, T, Gx, Gy, Gz;
  
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H);
  
  //divide each with their sensitivity scale factor
 // Ax = (double)AccelX/AccelScaleFactor;
 // Ay = (double)AccelY/AccelScaleFactor;
 // Az = (double)AccelZ/AccelScaleFactor;
 // T = (double)Temperature/340+36.53; //temperature formula
 // Gx = (double)GyroX/GyroScaleFactor;
 // Gy = (double)GyroY/GyroScaleFactor;
 // Gz = (double)GyroZ/GyroScaleFactor;

  //Serial.print(" Ax:"); Serial.print(Ax);
  //Serial.print(" Ay: "); Serial.print(Ay);
  //Serial.print(" Az: "); Serial.print(Az);
  //Serial.print(" T: "); Serial.print(T);
  //Serial.print(" Gx: "); Serial.print(Gx);
  //Serial.print(" Gy: "); Serial.print(Gy);
  //Serial.print(" Gz: "); Serial.println(Gz);
}

void ParseImuData (void)
{
  memcpy(packetBuffer,&imuData,sizeof(imuData));
}

void ParseGpsData (void)
{
  memcpy((packetBuffer+sizeof(imuData)),&gpsData,sizeof(gpsData));
}

void PrintVariables (void)
{
  imuData_t imuDataToPrint;
  gpsData_t gpsDataToPrint;
  String gpsString;
  String imuString;
  memcpy(&imuDataToPrint,packetBuffer,sizeof(imuData_t));
  memcpy(&gpsDataToPrint,packetBuffer+sizeof(imuData_t),sizeof(gpsData_t));
  Serial.print(imuDataToPrint.accelX); Serial.print(" ");
  Serial.print(imuDataToPrint.accelY); Serial.print(" ");
  Serial.print(imuDataToPrint.accelZ); Serial.print(" ");
  Serial.print(imuDataToPrint.gyroX);  Serial.print(" ");
  Serial.print(imuDataToPrint.gyroY);  Serial.print(" ");
  Serial.print(imuDataToPrint.gyroZ);  Serial.print(" ");
  gpsString = String(gpsDataToPrint.latitude, 6);
  Serial.print(gpsString); Serial.print(" ");
  gpsString = String(gpsDataToPrint.longitude, 6);
  Serial.print(gpsString); Serial.print(" ");
  Serial.print(gpsDataToPrint.hour); Serial.print(" ");
  Serial.print(gpsDataToPrint.minute); Serial.print(" ");
  Serial.print(gpsDataToPrint.second); Serial.print(" ");
  Serial.println(gpsDataToPrint.centisecond); Serial.print(" ");
}
void SendBufferUdp (void)
{
  ParseImuData();
  ParseGpsData();
  //PrintVariables();
  Udp.beginPacket(remoteIP, 56210);
  Udp.write(packetBuffer);
  Udp.endPacket();
}

void SerialConsole (void)
{
static int screenPosition;
static int screenLine;

}

/*****************************************************************/
/*                  Public Function Declaration                  */
/*****************************************************************/

void setup()
{
  //Serial.begin(9600);
  ss.begin(9600);
  Wire.begin(sda, scl);
  pinMode(D4, OUTPUT);
  digitalWrite(D4, HIGH);
  MPU6050_Init();
  WiFi.mode(WIFI_STA);
  WiFi.config(localIp, gateway, subnet);
  WiFi.begin(ssid, password);
  //Serial.print("Conectando a:\t");
    while (WiFi.status() != WL_CONNECTED) 
  {
    delay(200); 
   //Serial.print('.');
  }
  //Serial.println(ssid);
  Udp.begin(localPort);
}

void loop()
{
  if ((millis() & 0x7f) == 0x64)
    {
      ImuHandle();
      SendBufferUdp ();
    }
  GpsHandle();
}

