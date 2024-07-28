//
// Created by Administrator on 2024/7/22.
//

#ifndef CW2015_H
#define CW2015_H

#define CW2015_I2C_NUM I2C_NUM_0
#define CW2015_SDA_IO 0     /*!< gpio number for SDA of I2C peripheral */
#define CW2015_SCL_IO 1     /*!< gpio number for SCL of I2C peripheral */
#define CW2015_ADDR 0xC5

#define CW2015_WRITE_REG 0xC4
#define CW2015_READ_REG 0xC5
#define CW_VCELL_H        0x02 // 高字节电压寄存器地址
#define CW_VCELL_L        0x03 // 低字节电压寄存器地址

esp_err_t read_cw2015_battery_quantity(uint32_t *quantity);


#endif //CW2015_H
