#include <ESP8266WiFi.h>
#include <Wire.h>
#include <dahal_i2c.h>

// Select SDA and SCL pins for I2C communication 
const uint8_t scl = D6;
const uint8_t sda = D7;

static bool dahalI2cInitialized = false;

void DahalI2cInit(void)
{
    if (!dahalI2cInitialized)
    {
        Wire.begin(sda, scl);
        dahalI2cInitialized = true;
    }
}
void DahalI2cWrite(uint8_t deviceAddress, uint8_t regAddress, uint8_t data)
{
    Wire.beginTransmission(deviceAddress);
    Wire.write(regAddress);
    Wire.write(data);
    Wire.endTransmission();
}

void DahalI2cRead(uint8_t deviceAddress, uint8_t regAddress, uint8_t *data, uint8_t length)
{
    uint8_t index = 0;
    uint8_t *dataPointer;
    Wire.beginTransmission(deviceAddress);
    Wire.write(regAddress);
    Wire.endTransmission();
    Wire.requestFrom(deviceAddress, length);
    while (index<length)
    {
        dataPointer = data+index;
        *dataPointer = Wire.read();
        index++;
    }
}
