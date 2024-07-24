//
// Created by Administrator on 2024/7/22.
//

#ifndef CW2015_H
#define CW2015_H

#define CW2015_I2C_NUM I2C_NUM_0
#define CW2015_SDA_IO 0     /*!< gpio number for SDA of I2C peripheral */
#define CW2015_SCL_IO 1     /*!< gpio number for SCL of I2C peripheral */
#define CW2015_ADDR (0x4B << 1) /*!< I2C address of the CW2015 chip */

#define CW2015_BATTERY_QUANTITY_REG 0x0A


esp_err_t read_cw2015_battery_quantity(float *quantity);


#endif //CW2015_H
