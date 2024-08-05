//
// Created by Administrator on 2024/7/22.
//

#ifndef CW2015_H
#define CW2015_H

#define CW2015_I2C_NUM I2C_NUM_0
#define CW2015_SDA_IO 0     /*!< gpio number for SDA of I2C peripheral */
#define CW2015_SCL_IO 1     /*!< gpio number for SCL of I2C peripheral */

#define CW_ADDR       0X62
#define CW_VERSION    0x00
#define CW_VCELL      0x02
#define CW_SOC        0x04
#define CW_RRT_ALET   0x06
#define CW_CONFIG     0x08
#define CW_MODE       0x0a
#define byte uint8_t

void cw_2015_start();
void cw_2015_low_power_reset();
esp_err_t read_cw2015_battery_quantity(int *quantity);

void readAnalogVoltage(float *q);

uint8_t readRegister(uint8_t reg);

void i2c_write(uint8_t addr, uint8_t reg, uint8_t *data, size_t len);

void i2c_read(uint8_t addr, uint8_t reg, uint8_t *data, size_t len);

uint8_t cw_release_alrt_pin(void); //释放alrt pin,触发低电量中断后必须调用
#endif //CW2015_H
