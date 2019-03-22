#ifndef dahal_i2c_h
#define dahal_i2c_h


void DahalI2cInit(void);
void DahalI2cWrite(uint8_t deviceAddress, uint8_t regAddress, uint8_t data);
void DahalI2cRead(uint8_t deviceAddress, uint8_t regAddress, uint8_t *data, uint8_t length);


#endif