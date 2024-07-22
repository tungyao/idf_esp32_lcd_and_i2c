#include "cw2015.h"


void read_vol() {
    uint8_t read_data[2];
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (CW_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_read(cmd, read_data, 2, I2C_MASTER_ACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);


    float voltage = (read_data[0] << 8) | read_data[1];
    voltage /= 1000; // 转换为伏特
    int capacity = (read_data[0] * 256 + read_data[1]) / 256;
}

void read_analog_voltage() {
    uint16_t AnalogVoltage = 0x0000;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (CW_ADDR << 1) | I2C_MASTER_READ, true);
    i2c_master_write_byte(cmd,CW_VCELL | I2C_MASTER_WRITE, true);
    i2c_master_stop(cmd);
    i2c_master_write_to_device(I2C_NUM_0,CW_ADDR,CW_VCELL ,2, 0);
    // _pWire->endTransmission();
    // _pWire->requestFrom(CW_ADDR, (uint8_t) 2);
    // AnalogVoltage = _pWire->read();
    AnalogVoltage <<= 8;
}
