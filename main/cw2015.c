#include "esp_log.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "cw2015.h"

#include <string.h>


static const char *TAG = "CW2015_EXAMPLE";
// 转换电压值到实际电压
static float convert_voltage(uint16_t voltage_raw) {
    const float voltage_per_lsb = 0.000305; // 每单位代表的电压值
    return voltage_raw * voltage_per_lsb;
}

esp_err_t read_cw2015_battery_quantity(uint32_t *quantity) {
    uint8_t data[2];
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, CW2015_ADDR << 1 | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, CW_VCELL_H, true); // 写入高字节寄存器地址
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, CW2015_ADDR << 1 | I2C_MASTER_READ, true);
    i2c_master_read(cmd, data, 2, I2C_MASTER_ACK); // 先读取高字节，再读取低字节
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    *quantity = convert_voltage(data);
    i2c_cmd_link_delete(cmd);
    ESP_LOGI(TAG, "Battery quantity: %d", (int)*quantity);
    if (ret == ESP_OK) {
        // 由于我们先读取的是高字节，所以data[0]是高字节，data[1]是低字节
        return (data[0] << 8) | data[1];
    } else {
        return 0;
    }
}
