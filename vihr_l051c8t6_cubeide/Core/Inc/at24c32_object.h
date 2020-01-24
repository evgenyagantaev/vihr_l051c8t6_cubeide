#ifndef AT24C32_OBJECT_H
#define AT24C32_OBJECT_H

//#define AT24C32_ADDRESS 0x50
#define AT24C32_ADDRESS 0x51

static I2C_HandleTypeDef *at24c32_i2c_handle;

uint8_t at24c32_shifted_address = AT24C32_ADDRESS << 1;

#endif
