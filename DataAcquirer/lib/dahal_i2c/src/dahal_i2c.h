#ifndef dahal_i2c_h
#define dahal_i2c_h

class DahalI2c
{
  public:
    DahalI2c();
    void write(uint8_t deviceAddress, uint8_t regAddress, uint8_t data);
    void read(uint8_t deviceAddress, uint8_t regAddress, uint8_t *data, uint8_t length);
  private:
    int _pin;
};

#endif