//
// Created by Administrator on 2024/7/30.
//


#include "sleep.h"

void deep_sleep_register_gpio_wakeup(void) {
    const gpio_config_t config = {
        .pin_bit_mask = BIT(DEFAULT_WAKEUP_PIN),
        .mode = GPIO_MODE_INPUT,
    };

    ESP_ERROR_CHECK(gpio_config(&config));
    ESP_ERROR_CHECK(esp_deep_sleep_enable_gpio_wakeup(BIT(DEFAULT_WAKEUP_PIN), 1));
    xTaskCreate(deep_sleep_task, "deep_sleep_task", 4096, NULL, 6, NULL);
    printf("Enabling GPIO wakeup on pins GPIO%d\n", DEFAULT_WAKEUP_PIN);
}

static void deep_sleep_task(void *args) {
    // switch (esp_sleep_get_wakeup_cause()) {
    //     case ESP_SLEEP_WAKEUP_GPIO:
    //         uint64_t wakeup_pin_mask = esp_sleep_get_gpio_wakeup_status();
    //         if (wakeup_pin_mask != 0) {
    //             int pin = __builtin_ffsll(wakeup_pin_mask) - 1;
    //             printf("Wake up from GPIO %d\n", pin);
    //         } else {
    //             printf("Wake up from GPIO\n");
    //         }
    //         break;
    // }
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    // esp_deep_sleep_start();
}
