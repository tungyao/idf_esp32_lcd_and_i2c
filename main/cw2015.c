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

esp_err_t read_cw2015_battery_quantity(int *quantity) {
    uint8_t data[2];
    data[0] = readRegister(CW_VERSION);
    *quantity = data[0];
    return 0;
}

void readAnalogVoltage(float *q) {
    uint8_t data[2];
    memset(data, 0, 2);

    uint16_t AnalogVoltage = 0x0000;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd); //起始信号
    i2c_master_write_byte(cmd, (CW_ADDR << 1) | I2C_MASTER_WRITE, 1); //从机地址及读写位
    i2c_master_write_byte(cmd, CW_VCELL, 1); //数据位(数组)
    i2c_master_stop(cmd); //终止信号
    i2c_master_cmd_begin(0, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);


    cmd = i2c_cmd_link_create();
    //添加各种子数据帧
    i2c_master_start(cmd); //起始信号
    i2c_master_write_byte(cmd, (CW_ADDR << 1) | I2C_MASTER_READ, 1); //从机地址及读写位
    i2c_master_read(cmd, data, 2, 1);
    i2c_master_stop(cmd); //终止信号
    //向I2C_NUM_0 发送这个数据帧，timeout设置为1000毫秒
    i2c_master_cmd_begin(0, cmd, 1000 / portTICK_PERIOD_MS);
    //删除i2c_cmd_handle_t对象，释放资源
    i2c_cmd_link_delete(cmd);
    AnalogVoltage = ((data[0] << 8) + data[1]) & 0x3fff;
    *q = (float)AnalogVoltage * 0.000305;
}

uint8_t readRegister(uint8_t reg) //读寄存器，单字节
{
    uint8_t data[2];
    memset(data, 0, sizeof(data));

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    //创建i2c_cmd_handle_t对象cmd = i2c_cmd_link_create();
    //添加各种子数据帧
    i2c_master_start(cmd); //起始信号
    i2c_master_write_byte(cmd, (CW_ADDR << 1) | I2C_MASTER_WRITE, 1); //从机地址及读写位
    i2c_master_write_byte(cmd, reg, 1); //数据位(数组)
    i2c_master_stop(cmd); //终止信号
    //向I2C_NUM_0 发送这个数据帧，timeout设置为1000毫秒
    i2c_master_cmd_begin(0, cmd, 1000 / portTICK_PERIOD_MS);
    //删除i2c_cmd_handle_t对象，释放资源
    i2c_cmd_link_delete(cmd);

    cmd = i2c_cmd_link_create();
    //添加各种子数据帧
    i2c_master_start(cmd); //起始信号
    i2c_master_write_byte(cmd, (CW_ADDR << 1) | I2C_MASTER_READ, 1); //从机地址及读写位
    i2c_master_read(cmd, data, 1, 1);
    i2c_master_stop(cmd); //终止信号
    //向I2C_NUM_0 发送这个数据帧，timeout设置为1000毫秒
    i2c_master_cmd_begin(0, cmd, 1000 / portTICK_PERIOD_MS);
    //删除i2c_cmd_handle_t对象，释放资源
    i2c_cmd_link_delete(cmd);
    return data[0];
}
