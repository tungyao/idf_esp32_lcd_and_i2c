#include "esp_log.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "cw2015.h"


static const char *TAG = "CW2015_EXAMPLE";

esp_err_t read_cw2015_battery_quantity(float *quantity) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (CW2015_ADDR | I2C_MASTER_WRITE), true);
    i2c_master_write_byte(cmd, CW2015_BATTERY_QUANTITY_REG, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(CW2015_I2C_NUM, cmd, 1000 / portTICK_PERIOD_MS);

    uint8_t l = 0;
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (CW2015_ADDR | I2C_MASTER_READ), true);
    i2c_master_read_byte(cmd, &l, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(CW2015_I2C_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
    *quantity = (l / 256.0f) * 100.0f;
    return ret;
}
