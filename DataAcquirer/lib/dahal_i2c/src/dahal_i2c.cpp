#include <ESP8266WiFi.h>
#include <Wire.h>
#include <dahal_i2c.h>

// Select SDA and SCL pins for I2C communication 
const uint8_t scl = D6;
const uint8_t sda = D7;

static bool dahalI2cInitialized = false;

void DahalI2cInit (void);
void DahalI2cInit (void)
{
    Wire.begin(sda, scl);
}

DahalI2c::DahalI2c()
{
    if (!dahalI2cInitialized)
    {
        DahalI2cInit();
        dahalI2cInitialized = true;
    }
}
void DahalI2c::write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data)
{
    Wire.beginTransmission(deviceAddress);
    Wire.write(regAddress);
    Wire.write(data);
    Wire.endTransmission();
}

void DahalI2c::read(uint8_t deviceAddress, uint8_t regAddress, uint8_t *data, uint8_t length)
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
