#include <aht20.h>
#include <stdio.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_vendor.h>
#include <esp_sntp.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_task_wdt.h>
#include <esp_timer.h>
#include <core/lv_disp.h>
#include <core/lv_obj.h>
#include <driver/spi_common.h>
#include <driver/temperature_sensor.h>
#include <driver/uart.h>
#include <extra/widgets/meter/lv_meter.h>
#include <hal/lv_hal_disp.h>
#include <widgets/lv_label.h>

#include "conn.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "cw2015.h"

// 初始化i2c
static const char *TAG = "test1";

#define I2C_MASTER_SCL_IO           1      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           0      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */



#define I2C_MASTER_FREQ_HZ          100000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

// aht20
#define AHT20_ADDR 0x38
#define AHT20_PIN  12
#include "panel1.h"
#include "sto.h"

void task_aht20(void *pvParameters);

void task_lvgl(void *pvParameters);

void task_listen_key(void *pv);


void task_time(void *pv);

static esp_err_t i2c_master_init(void) {
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}


static uint32_t temperature_raw;
static uint32_t humidity_raw;
static float temperature;
static float humidity;

void task_bat(void *pv);

#define s5 vTaskDelay(pdMS_TO_TICKS(5000))
#define s1 vTaskDelay(pdMS_TO_TICKS(1000))
static aht20_dev_handle_t aht20 = NULL;
static temperature_sensor_handle_t temp_sensor = NULL;

static void aht20_read_data() {
    aht20_read_temperature_humidity(aht20, &temperature_raw, &temperature, &humidity_raw, &humidity);
    float tsens_value;
    ESP_ERROR_CHECK(temperature_sensor_get_celsius(temp_sensor, &tsens_value));
    temperature = temperature + 0.9 * (tsens_value - temperature);
}


/* Rotate display and touch, when rotated screen in LVGL. Called when driver parameters are updated. */
void app_main(void) {
    // 初始化内部存储
    conn_keys_init();
    sto_init();
    i2c_master_init();


    temperature_sensor_config_t temp_sensor_config = TEMPERATURE_SENSOR_CONFIG_DEFAULT(10, 50);
    ESP_ERROR_CHECK(temperature_sensor_install(&temp_sensor_config, &temp_sensor));
    ESP_LOGI(TAG, "Enable temperature sensor");
    ESP_ERROR_CHECK(temperature_sensor_enable(temp_sensor));
    ESP_LOGI(TAG, "Read temperature");

    gpio_config_t aht20_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << AHT20_PIN
    };
    ESP_ERROR_CHECK(gpio_config(&aht20_gpio_config));
    gpio_set_level(AHT20_PIN, 1);

    init_lcd();
    lv_obj_t *scr = lv_disp_get_scr_act(get_disp());


    // Set UART configuration
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_0, &uart_config);

    // Install UART driver
    uart_driver_install(UART_NUM_0, 128 * 2, 0, 0, NULL, 0);


    // AHT20 INIT
    aht20_i2c_config_t i2c_conf = {
        .i2c_port = I2C_MASTER_NUM,
        .i2c_addr = AHT20_ADDRRES_0,
    };
    aht20_new_sensor(&i2c_conf, &aht20);


    xTaskCreate(task_lvgl, "lvgl", 80960, scr, 1,NULL);
    xTaskCreate(task_aht20, "aht20", 4096,NULL, 10,NULL);
    // xTaskCreate(task_listen_key, "listen", 4096,NULL, 1,NULL);
    // xTaskCreate(listen_uart, "uart", 4096,NULL, 24,NULL);
    xTaskCreate(task_bat, "bat", 1024,NULL, 24,NULL);
    xTaskCreate(task_time, "time", 4096,NULL, 15,NULL);
    // task_conn(NULL);
}

void task_aht20(void *pvParameters) {
    while (1) {
        aht20_read_data();

        s5;
    }
}


void task_lvgl(void *pvParameters) {
    panel3(pvParameters);
    panel2(pvParameters);
    panel1(pvParameters);
    set_weather(
        "{\"temp\":36,\"feelsLike\":37,\"text\":\"多云\",\"text_icon\":1,\"humidity\":45,\"vis\":30,\"cloud\":100}");
    while (1) {
        // lv_label_set_text_fmt(temp_label, "%-5s:  %2.2f deg", "temp", temperature);
        // lv_label_set_text_fmt(humid_label, "%-5s: %2.2f %%", "humidity", humidity);
        update_meter_value(temperature);
        update_text_temp(temperature);
        update_text_humid(humidity);
        update_emoji(temperature, humidity);
        // raise the task priority of LVGL and/or reduce the handler period can improve the performance
        vTaskDelay(pdMS_TO_TICKS(1000));
        // The task running lv_timer_handler should have lower priority than that running `lv_tick_inc`
        lv_timer_handler();
    }
}


// 增加两个按键的控制


void task_listen_key(void *pv) {
    listen_config_key();
}


static uint32_t bat;

void task_bat(void *pv) {
    while (1) {
        read_cw2015_battery_quantity(&bat);
        update_bat(bat);
        s5;
    }
}

void task_time(void *pv) {
    // 将时区设置为中国标准时间

    setenv("TZ", "CST-8", 1);
    tzset();
    while (1) {
        ESP_LOGI(TAG, "sntp status %d", sntp_get_sync_status());
        if (sntp_get_sync_status() == SNTP_SYNC_STATUS_COMPLETED) {
            time_t rawtime;
            time(&rawtime);
            struct tm* timeinfo = localtime(&rawtime);
            update_time(timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
        }
        s1;
    }
}
