#include <stdio.h>
#include "conn.h"

#include <esp_timer.h>
#include <driver/gpio.h>
#include <freertos/projdefs.h>


void conn_keys_init() {
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE, // Disable interrupt
        .mode = GPIO_MODE_INPUT, // Set as Input
        .pin_bit_mask = (1ULL << IO19), // Bitmask
        .pull_up_en = GPIO_PULLUP_ENABLE, // Enable pull-up
    };
    gpio_config(&io_conf);

    gpio_config_t io_conf2 = {
        .intr_type = GPIO_INTR_DISABLE, // Disable interrupt
        .mode = GPIO_MODE_INPUT, // Set as Input
        .pin_bit_mask = (1ULL << IO18), // Bitmask
        .pull_up_en = GPIO_PULLUP_ENABLE, // Enable pull-up
    };
    gpio_config(&io_conf2);
}

bool listen_config_key() {


    int64_t press_start_time = esp_timer_get_time(); // 获取当前时间
    while (1) {
        if (gpio_get_level(IO19) == 1) { // 检测按键是否被按下
            vTaskDelay(pdMS_TO_TICKS(100)); // 短暂延时，消抖
            if (gpio_get_level(IO19) == 1) { // 再次检测，确认按键状态
                int64_t current_time = esp_timer_get_time();
                if ((current_time - press_start_time) >= LONG_PRESS_TIME) {
                    return true; // 长按时间达到阈值
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20)); // 延时，降低CPU占用
    }
    return false;
}

void get_uart_data() {
    
}