#include "esp_log.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "cw2015.h"

#include <string.h>


static const char *TAG = "CW2015_EXAMPLE";

esp_err_t read_cw2015_battery_quantity(uint32_t *quantity) {
    uint8_t data[2];
    memset(data, 0, 2);
    uint8_t reg = CW2015_WRITE_REG;
    uint8_t reg_r = CW2015_READ_REG;
    i2c_master_write_to_device(I2C_NUM_0, reg, (uint8_t *) 0x02, 1, 1000 / portTICK_PERIOD_MS);
    i2c_master_read_from_device(I2C_NUM_0, reg_r, (uint8_t *) data[0], 1, 1000 / portTICK_PERIOD_MS);
    i2c_master_write_to_device(I2C_NUM_0, reg, (uint8_t *) 0x03, 1, 1000 / portTICK_PERIOD_MS);
    i2c_master_read_from_device(I2C_NUM_0, reg_r, (uint8_t *) data[1], 1, 1000 / portTICK_PERIOD_MS);
    uint32_t ad_buff = 0;
    ad_buff = (data[0] << 8) + data[1];
    *quantity = ad_buff * 305 / 1000;
    return 0;
}
