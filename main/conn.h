#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/gpio.h"
static const char *CONN_TAG = "conn";

#define EX_UART_NUM UART_NUM_0
#define PATTERN_CHR_NUM    (3)         /*!< Set the number of consecutive and identical characters received by receiver which defines a UART pattern*/


#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)

static QueueHandle_t uart0_queue;

#define IO19 19
#define IO18 18

#define LONG_PRESS_TIME 3000


void conn_keys_init();

// 监听一个按键
bool listen_config_key();

void get_uart_data();
