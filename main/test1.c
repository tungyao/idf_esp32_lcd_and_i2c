#include <stdio.h>
#include <aht20.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_vendor.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_task_wdt.h>
#include <core/lv_obj.h>
#include <driver/spi_common.h>
#include <hal/lv_hal_disp.h>
#include "driver/i2c.h"
#include "esp_log.h"
// 初始化i2c
static const char *TAG = "test1";

#define I2C_MASTER_SCL_IO           1      /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           0      /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */



#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000

#define MPU9250_SENSOR_ADDR                 0x68        /*!< Slave address of the MPU9250 sensor */
#define MPU9250_WHO_AM_I_REG_ADDR           0x75        /*!< Register addresses of the "who am I" register */

#define MPU9250_PWR_MGMT_1_REG_ADDR         0x6B        /*!< Register addresses of the power managment register */
#define MPU9250_RESET_BIT                   7


// aht20
static aht20_dev_handle_t aht20 = NULL;

void task_aht20(void *pvParameters);

/**
 * @brief i2c master initialization
 */
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

#define s5 vTaskDelay(pdMS_TO_TICKS(5000))
#define s1 vTaskDelay(pdMS_TO_TICKS(1000))

void init_aht20() {
    aht20_i2c_config_t i2c_conf = {
        .i2c_port = I2C_MASTER_NUM,
        .i2c_addr = AHT20_ADDRRES_0,
    };
    aht20_new_sensor(&i2c_conf, &aht20);

    aht20_read_temperature_humidity(aht20, &temperature_raw, &temperature, &humidity_raw, &humidity);
    ESP_LOGI(TAG, "%-20s: %2.2f %%", "humidity is", humidity);
    ESP_LOGI(TAG, "%-20s: %2.2f degC", "temperature is", temperature);
    aht20_del_sensor(aht20);
}

#define LCD_H_RES              240
#define LCD_V_RES              320
#define LCD_PIXEL_CLOCK_HZ     (20 * 1000 * 1000)
#define LCD_CMD_BITS           8
#define LCD_PARAM_BITS         8
#define GPIO_HANDSHAKE      2
#define GPIO_MOSI           5
#define GPIO_MISO           -1
#define GPIO_SCLK           3
#define GPIO_CS             4
#define LCD_DC              6
#define GPIO_RST            -1
#define LCD_BL              2

static bool notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata,
                                            void *user_ctx) {
    lv_disp_drv_t *disp_driver = (lv_disp_drv_t *) user_ctx;
    lv_disp_flush_ready(disp_driver);
    return false;
}

static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map)
{
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) drv->user_data;
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    // copy a buffer's content to a specific area of the display
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
}

void app_main(void) {
    i2c_master_init();
    esp_task_wdt_config_t wdt_config = {
        .timeout_ms = 5,
        .idle_core_mask = 1 << 2,
        .trigger_panic = false
    };
    esp_task_wdt_init(&wdt_config);
    xTaskCreate(task_aht20, "aht20", 4096,NULL, 1,NULL);


    esp_lcd_panel_handle_t panel;
    esp_lcd_panel_dev_config_t lcd_conf = {
        .reset_gpio_num = GPIO_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    spi_bus_config_t buscfg = {
        .mosi_io_num = GPIO_MOSI,
        .miso_io_num = GPIO_MISO,
        .sclk_io_num = GPIO_SCLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_H_RES * 80 * sizeof(uint16_t),

    };
    spi_bus_initialize(SPI1_HOST, &buscfg, SPI_DMA_CH_AUTO);


    static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
    static lv_disp_drv_t disp_drv; // contains callback functions
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = LCD_DC,
        .cs_gpio_num = GPIO_CS,
        .pclk_hz = LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = LCD_CMD_BITS,
        .lcd_param_bits = LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 10,
        .on_color_trans_done = notify_lvgl_flush_ready,
        .user_ctx = &disp_drv,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI1_HOST, &io_config, &io_handle));
    esp_lcd_new_panel_st7789(io_handle, &lcd_conf, &panel);
    esp_lcd_panel_reset(panel);
    esp_lcd_panel_init(panel);

    esp_lcd_panel_disp_on_off(panel, true);
    gpio_set_level(LCD_BL, 1);

    lv_init();
    lv_color_t *buf1 = heap_caps_malloc(LCD_H_RES * 20 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1);
    lv_color_t *buf2 = heap_caps_malloc(LCD_H_RES * 20 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf2);
    lv_disp_drv_init(&disp_drv);

    disp_drv.hor_res = LCD_H_RES;
    disp_drv.ver_res = LCD_V_RES;
    disp_drv.flush_cb = lvgl_flush_cb;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = panel;
    lv_disp_t *disp = lv_disp_drv_register(&disp_drv);

}


void task_aht20(void *pvParameters) {
    while (1) {
        init_aht20();
        s5;
    }
}
