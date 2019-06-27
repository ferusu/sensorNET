#include "daman_imu.h"
#include <Wire.h>

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


static int16_t Temperature;

void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data);
void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress);
void ImuOffsetAdjustment (void);
void MPU6050_Init(void);

void I2C_Write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data)
{
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.write(data);
  Wire.endTransmission();
}

void Read_RawValue(uint8_t deviceAddress, uint8_t regAddress, packet_t *packet)
{
  Wire.beginTransmission(deviceAddress);
  Wire.write(regAddress);
  Wire.endTransmission();
  Wire.requestFrom(deviceAddress, (uint8_t)14);
  packet->accelX = (((int16_t)Wire.read()<<8) | Wire.read());
  packet->accelY = (((int16_t)Wire.read()<<8) | Wire.read());
  packet->accelZ = (((int16_t)Wire.read()<<8) | Wire.read());
  Temperature = (((int16_t)Wire.read()<<8) | Wire.read());
  packet->gyroX = (((int16_t)Wire.read()<<8) | Wire.read());
  packet->gyroY = (((int16_t)Wire.read()<<8) | Wire.read());
  packet->gyroZ = (((int16_t)Wire.read()<<8) | Wire.read());
}

void ImuOffsetAdjustment (packet_t *packet)
{
  packet->accelX = packet->accelX + accelXOffset;
  packet->accelY = packet->accelY + accelYOffset;
  packet->accelZ = packet->accelZ + accelZOffset;
  packet->gyroX = packet->gyroX + gyroXOffset;
  packet->gyroY = packet->gyroY + gyroYOffset;
  packet->gyroZ = packet->gyroZ + gyroZOffset;
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

void ImuHandle (packet_t *packet)
{
  Read_RawValue(MPU6050SlaveAddress, MPU6050_REGISTER_ACCEL_XOUT_H, packet);
  //ImuOffsetAdjustment();
}

void ImuInit (void)
{
  Wire.begin(sda, scl);
  MPU6050_Init();
}