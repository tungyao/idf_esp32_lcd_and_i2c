//
// Created by Administrator on 2024/7/30.
//

#ifndef SLEEP_H
#define SLEEP_H
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include <time.h>
#include <driver/gpio.h>
#include <sys/time.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"

#include "esp_sleep.h"
#include "esp_log.h"
void deep_sleep_register_gpio_wakeup(void);
static void deep_sleep_task(void *args);

#define DEFAULT_WAKEUP_PIN 19

#endif //SLEEP_H
