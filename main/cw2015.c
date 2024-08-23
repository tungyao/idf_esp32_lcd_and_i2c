#include "esp_log.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "cw2015.h"

#include <string.h>

int voltageToPercentage(float voltage) {
    // 定义满电和最低电压
    const float FULL_VOLTAGE = 4.2; // 满电电压
    const float MIN_VOLTAGE = 3.7; // 最低电压

    // 计算电压范围
    float voltageRange = FULL_VOLTAGE - MIN_VOLTAGE;

    // 防止电压超出合理范围
    if (voltage > FULL_VOLTAGE) {
        voltage = FULL_VOLTAGE;
    } else if (voltage < MIN_VOLTAGE) {
        voltage = MIN_VOLTAGE;
    }

    // 计算电量百分比
    int percentage = (int) (((voltage - MIN_VOLTAGE) / voltageRange) * 100);

    return percentage;
}

static const char *TAG = "CW2015_EXAMPLE";
// 转换电压值到实际电压
static float convert_voltage(uint16_t voltage_raw) {
    const float voltage_per_lsb = 0.000305; // 每单位代表的电压值
    return voltage_raw * voltage_per_lsb;
}

void cw_2015_start() {
    uint8_t a[1];
    a[0] = 0x00;
    i2c_write(CW_ADDR, CW_MODE, a, 1);
}

void cw_2015_low_power_reset() {
    uint8_t a[1];

    int8_t ret = 0;
    uint8_t reg_val;
    i2c_read(CW_ADDR,CW_RRT_ALET, a, 1);
    if (a[0]) {
        return;
    }
    a[0] = a[0] & 0x80;
    a[0] = a[0] & 0x7f;
    i2c_write(CW_ADDR, CW_RRT_ALET, a, 1);
}

esp_err_t read_cw2015_battery_quantity(int *quantity) {
    // uint8_t data[2];
    // data[0] = readRegister(CW_SOC);
    // 阈值是3.3v 满电是3.7v
    float a;

    readAnalogVoltage(&a);
    *quantity = voltageToPercentage(a);
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
    *q = (float) AnalogVoltage * 0.000305;
}

uint8_t readRegister(uint8_t reg) //读寄存器，单字节
{
    uint8_t data[2];
    memset(data, 0, sizeof(data));

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd); //起始信号
    i2c_master_write_byte(cmd, (CW_ADDR << 1) | I2C_MASTER_WRITE, 1); //从机地址及读写位
    i2c_master_write_byte(cmd, reg, 1); //数据位(数组)
    i2c_master_stop(cmd); //终止信号
    i2c_master_cmd_begin(0, cmd, 1000 / portTICK_PERIOD_MS);
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

// 从某一个寄存器读
void i2c_read(uint8_t addr, uint8_t reg, uint8_t *data, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd); //起始信号
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, 1); //从机地址及读写位
    i2c_master_write_byte(cmd, reg, 1); //数据位(数组)
    i2c_master_stop(cmd); //终止信号
    i2c_master_cmd_begin(0, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    cmd = i2c_cmd_link_create();
    //添加各种子数据帧
    i2c_master_start(cmd); //起始信号
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_READ, 1); //从机地址及读写位
    i2c_master_read(cmd, data, len, 1);
    i2c_master_stop(cmd); //终止信号
    //向I2C_NUM_0 发送这个数据帧，timeout设置为1000毫秒
    i2c_master_cmd_begin(0, cmd, 1000 / portTICK_PERIOD_MS);
    //删除i2c_cmd_handle_t对象，释放资源
    i2c_cmd_link_delete(cmd);
}

// 向某一个寄存器写
void i2c_write(uint8_t addr, uint8_t reg, uint8_t *data, size_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    //添加各种子数据帧
    i2c_master_start(cmd); //起始信号
    i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, 1); //从机地址及读写位
    i2c_master_write_byte(cmd, reg, 1); //数据位(数组)
    for (int i = 0; i < len; ++i) {
        i2c_master_write_byte(cmd, data[i], 1); //数据位(数组)
    }
    i2c_master_stop(cmd); //终止信号
    //向I2C_NUM_0 发送这个数据帧，timeout设置为1000毫秒
    i2c_master_cmd_begin(0, cmd, 1000 / portTICK_PERIOD_MS);
    //删除i2c_cmd_handle_t对象，释放资源
    i2c_cmd_link_delete(cmd);
}
