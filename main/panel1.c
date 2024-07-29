#include "panel1.h"

#include <math.h>
#include <stdio.h>


#ifdef IDF_VER
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <driver/spi_common.h>

#include <esp_heap_caps.h>
#include <esp_lcd_panel_io.h>
#include <esp_lcd_panel_ops.h>
#include <esp_lcd_panel_vendor.h>
#include <esp_log.h>
#include <esp_netif_sntp.h>
#include <esp_sntp.h>
#include <esp_timer.h>
#include "conn.h"

static bool example_notify_lvgl_flush_ready(esp_lcd_panel_io_handle_t panel_io, esp_lcd_panel_io_event_data_t *edata,
                                            void *user_ctx) {
    lv_disp_drv_t *disp_driver = (lv_disp_drv_t *) user_ctx;
    lv_disp_flush_ready(disp_driver);
    return false;
}


static void example_increase_lvgl_tick(void *arg) {
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}


int read_key(void) {
    if (gpio_get_level(19) == 1) {
        if (!is_diplay) {
            return -1;
        }
        return LV_KEY_NEXT;
    } else if (gpio_get_level(18) == 1) {
        return LV_KEY_ENTER;
    } else {
        return -1;
    }
}

void button_read(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    static uint32_t last_btn = 0; /*Store the last pressed button*/
    int btn_pr = read_key(); /*Get the ID (0,1,2...) of the pressed button*/
    if (btn_pr >= 0) {
        /*Is there a button press? (E.g. -1 indicated no button was pressed)*/
        last_btn = btn_pr; /*Save the ID of the pressed button*/
        data->state = LV_INDEV_STATE_PRESSED; /*Set the pressed state*/
    } else {
        data->state = LV_INDEV_STATE_RELEASED; /*Set the released state*/
    }

    data->key = last_btn; /*Save the last button*/
}


static void example_lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map) {
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) drv->user_data;
    int offsetx1 = area->x1;
    int offsetx2 = area->x2;
    int offsety1 = area->y1;
    int offsety2 = area->y2;
    // copy a buffer's content to a specific area of the display
    esp_lcd_panel_draw_bitmap(panel_handle, offsetx1, offsety1, offsetx2 + 1, offsety2 + 1, color_map);
}

static void example_lvgl_port_update_callback(lv_disp_drv_t *drv) {
    esp_lcd_panel_handle_t panel_handle = (esp_lcd_panel_handle_t) drv->user_data;

    switch (drv->rotated) {
        case LV_DISP_ROT_NONE:
            // Rotate LCD display
            esp_lcd_panel_swap_xy(panel_handle, false);
            esp_lcd_panel_mirror(panel_handle, true, false);
#if CONFIG_EXAMPLE_LCD_TOUCH_ENABLED
        // Rotate LCD touch
        esp_lcd_touch_set_mirror_y(tp, false);
        esp_lcd_touch_set_mirror_x(tp, false);
#endif
            break;
        case LV_DISP_ROT_90:
            // Rotate LCD display
            esp_lcd_panel_swap_xy(panel_handle, true);
            esp_lcd_panel_mirror(panel_handle, true, true);
#if CONFIG_EXAMPLE_LCD_TOUCH_ENABLED
        // Rotate LCD touch
        esp_lcd_touch_set_mirror_y(tp, false);
        esp_lcd_touch_set_mirror_x(tp, false);
#endif
            break;
        case LV_DISP_ROT_180:
            // Rotate LCD display
            esp_lcd_panel_swap_xy(panel_handle, false);
            esp_lcd_panel_mirror(panel_handle, false, true);
#if CONFIG_EXAMPLE_LCD_TOUCH_ENABLED
        // Rotate LCD touch
        esp_lcd_touch_set_mirror_y(tp, false);
        esp_lcd_touch_set_mirror_x(tp, false);
#endif
            break;
        case LV_DISP_ROT_270:
            // Rotate LCD display
            esp_lcd_panel_swap_xy(panel_handle, true);
            esp_lcd_panel_mirror(panel_handle, false, false);
#if CONFIG_EXAMPLE_LCD_TOUCH_ENABLED
        // Rotate LCD touch
        esp_lcd_touch_set_mirror_y(tp, false);
        esp_lcd_touch_set_mirror_x(tp, false);
#endif
            break;
    }
}

void init_lcd() {
    ESP_LOGI("PANEL", "Turn off LCD backlight");
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << EXAMPLE_PIN_NUM_BK_LIGHT
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));

    ESP_LOGI("PANEL", "Initialize SPI bus");
    spi_bus_config_t buscfg = {
        .sclk_io_num = EXAMPLE_PIN_NUM_SCLK,
        .mosi_io_num = EXAMPLE_PIN_NUM_MOSI,
        .miso_io_num = EXAMPLE_PIN_NUM_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = EXAMPLE_LCD_H_RES * 80 * sizeof(uint16_t),
    };
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO));

    ESP_LOGI("PANEL", "Install panel IO");
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = EXAMPLE_PIN_NUM_LCD_DC,
        .cs_gpio_num = EXAMPLE_PIN_NUM_LCD_CS,
        .pclk_hz = EXAMPLE_LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = EXAMPLE_LCD_CMD_BITS,
        .lcd_param_bits = EXAMPLE_LCD_PARAM_BITS,
        .spi_mode = 0,
        .trans_queue_depth = 10,
        .on_color_trans_done = example_notify_lvgl_flush_ready,
        .user_ctx = &disp_drv,
    };
    // Attach the LCD to the SPI bus
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST, &io_config, &io_handle));

    esp_lcd_panel_handle_t panel_handle = NULL;
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = EXAMPLE_PIN_NUM_LCD_RST,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };

    ESP_LOGI("PANEL", "Install ST7789 panel driver");
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));


    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, false, false));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));

    // user can flush pre-defined pattern to the screen before we turn on the screen or backlight
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));


    ESP_LOGI("PANEL", "Turn on LCD backlight");
    gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, 0);

    ESP_LOGI("PANEL", "Initialize LVGL library");
    lv_init();
    // alloc draw buffers used by LVGL
    // it's recommended to choose the size of the draw buffer(s) to be at least 1/10 screen sized
    lv_color_t *buf1 = heap_caps_malloc(EXAMPLE_LCD_H_RES * 20 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf1);
    lv_color_t *buf2 = heap_caps_malloc(EXAMPLE_LCD_H_RES * 20 * sizeof(lv_color_t), MALLOC_CAP_DMA);
    assert(buf2);
    // initialize LVGL draw buffers
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, EXAMPLE_LCD_H_RES * 20);
    // esp_lcd_panel_swap_xy(panel_handle, false);
    // esp_lcd_panel_mirror(panel_handle, true, true);
    ESP_LOGI("PANEL", "Register display driver to LVGL");
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = EXAMPLE_LCD_H_RES;
    disp_drv.ver_res = EXAMPLE_LCD_V_RES;
    disp_drv.draw_buf = &disp_buf;
    disp_drv.user_data = panel_handle;
    disp_drv.sw_rotate = 1;
    disp_drv.flush_cb = example_lvgl_flush_cb;
    disp_drv.drv_update_cb = example_lvgl_port_update_callback;
    disp_drv.rotated = LV_DISP_ROT_180;
    disp = lv_disp_drv_register(&disp_drv);
    // esp_lcd_panel_swap_xy(panel_handle, false);
    // esp_lcd_panel_mirror(panel_handle, true, true);
    ESP_LOGI("PANEL", "Install LVGL tick timer");
    esp_lcd_panel_swap_xy(panel_handle, true);
    esp_lcd_panel_mirror(panel_handle, false, true);
    // 注册回调按键汇报
    // lv_indev_t *btn_index = NULL;
    lv_indev_drv_init(&indev_drv_key);
    indev_drv_key.read_cb = button_read;
    indev_drv_key.type = LV_INDEV_TYPE_KEYPAD;
    btn_index = lv_indev_drv_register(&indev_drv_key);
    group = lv_group_create();
    lv_group_set_default(group);
    lv_indev_set_group(btn_index, group);
    // lv_group_focus_obj(group); //分组聚焦到对象
    lv_group_set_editing(group, true); //编辑模式


    const esp_timer_create_args_t lvgl_tick_timer_args = {
        .callback = &example_increase_lvgl_tick,
        .name = "lvgl_tick"
    };
    esp_timer_handle_t lvgl_tick_timer = NULL;
    ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(lvgl_tick_timer, 2 * 1000));

    ESP_LOGI("PANEL", "Display LVGL Meter Widget");


    // 创建一个隐形的按钮
}
#endif

static void next_panel_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        switch_panel();
    }
}

lv_disp_t *get_disp() {
    return disp;
}

void init_sim() {
    lv_obj_t *btn2 = lv_btn_create(lv_scr_act());
    lv_obj_add_event_cb(btn2, next_panel_cb, LV_EVENT_ALL, NULL);
    lv_obj_set_pos(btn2, 280, 10);
    lv_obj_t *label4 = lv_label_create(btn2);
    lv_label_set_text(label4, ">");
    lv_obj_center(label4);
#ifdef IDF_VER
    lv_group_add_obj(group, btn2);
    lv_group_focus_obj(btn2); //分组聚焦到对象
#endif
}

void meter1(lv_obj_t *scr) {
    // 创建中间那个温度数字
    temp_label = lv_label_create(scr);
    lv_obj_set_style_text_font(temp_label, &number_48px, LV_STATE_DEFAULT);
    lv_obj_align(temp_label, LV_TEXT_ALIGN_CENTER, 8, 120);
    lv_label_set_text_fmt(temp_label, "%d%s", 25, "°");

    // 显示temperature文字
    lv_obj_t *temp_label_t = lv_label_create(scr);
    lv_obj_set_style_text_font(temp_label_t, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(temp_label_t, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_align(temp_label_t, LV_TEXT_ALIGN_CENTER, 0, 166);
    lv_label_set_text(temp_label_t, "Temperature");


    // 显示温度计图标
    LV_IMG_DECLARE(humid_icon)
    lv_obj_t *img1 = lv_img_create(scr);
    lv_img_set_src(img1, &humid_icon);
    lv_obj_set_pos(img1, 40, 188);
    lv_obj_set_size(img1, 22, 22);


    // 创建湿度数字
    humid = lv_label_create(scr);
    lv_obj_set_style_text_font(humid, &lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_set_pos(humid, 65, 190);
    lv_label_set_text_fmt(humid, "%s", "90%");
    lv_obj_set_scrollbar_mode(scr, LV_SCROLLBAR_MODE_OFF);


    // 创建温湿度thi相关的
    emoji_obj = lv_img_create(scr);
    lv_img_set_src(emoji_obj, &g626);
    lv_obj_set_pos(emoji_obj, 250, 188);

    // 创建里面那层半圆点点
    g_meter2 = lv_meter_create(scr);
    lv_obj_remove_style(g_meter2, NULL, LV_PART_MAIN);
    lv_obj_remove_style(g_meter2, NULL, LV_PART_INDICATOR);
    lv_obj_align(g_meter2, LV_ALIGN_CENTER, 0, 50);
    lv_obj_set_size(g_meter2, 190, 190);
    scale2 = lv_meter_add_scale(g_meter2);
    lv_meter_set_scale_range(g_meter2, scale2, -10, 50, 180, 180);
    lv_meter_set_scale_ticks(g_meter2, scale2, 31, 2, 2, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(g_meter2, scale2, 15, 2, 5, lv_color_black(), 16);
    lv_meter_add_scale_lines(g_meter2, scale2, lv_palette_main(LV_PALETTE_GREEN),
                             lv_palette_main(LV_PALETTE_PURPLE),
                             0, 0);


    // 创建外面那层半圆
    g_meter = lv_meter_create(scr);
    lv_obj_remove_style(g_meter, NULL, LV_PART_MAIN);
    lv_obj_remove_style(g_meter, NULL, LV_PART_INDICATOR);
    lv_obj_align(g_meter, LV_ALIGN_CENTER, 0, 50);
    lv_obj_set_size(g_meter, 250, 250);
    scale1 = lv_meter_add_scale(g_meter);
    lv_meter_set_scale_range(g_meter, scale1, -10, 50, 180, 180);
    lv_color_t start_color = lv_color_hex(0x09ece0);
    lv_color_t end_color = lv_color_hex(0xff8040);
    lv_meter_set_scale_ticks(g_meter, scale1, 31, 2, 25, lv_palette_main(LV_PALETTE_GREY));
    indix = lv_meter_add_scale_lines(g_meter, scale1, start_color,
                                     end_color,
                                     0, 0);
    lv_meter_set_indicator_start_value(g_meter, indix, -10);
    lv_meter_set_indicator_end_value(g_meter, indix, 50);
    lv_color_t my_color = lv_color_hex(0xEEE9E9);
    // lv_color_t my_color = lv_palette_main(LV_PALETTE_GREY);
    indix = lv_meter_add_scale_lines(g_meter, scale1, my_color,
                                     my_color,
                                     0, 0);
}

void meter2(lv_obj_t *scr) {
    humid = lv_label_create(scr);
    lv_obj_set_style_text_font(humid, &lv_font_montserrat_32, LV_STATE_DEFAULT);
    lv_obj_align(humid, LV_TEXT_ALIGN_CENTER, 10, 40);
    lv_label_set_text_fmt(humid, "%-5s:  %s", "humid", "loading");
    g_meter2 = lv_meter_create(scr);
    lv_obj_set_pos(g_meter2, 160, 80);
    lv_obj_set_size(g_meter2, 150, 150);
    scale2 = lv_meter_add_scale(g_meter2);
    lv_meter_set_scale_range(g_meter2, scale2, 0, 100, 300, 120);
    lv_meter_set_scale_ticks(g_meter2, scale2, 101, 2, 5, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(g_meter2, scale2, 10, 2, 12, lv_color_black(), 12);
    indix2 = lv_meter_add_needle_line(g_meter2, scale2, 3, lv_palette_main(LV_PALETTE_ORANGE), -10);
}


void update_meter_value(int32_t value) {
    if (g_meter == NULL) return;
    lv_meter_set_indicator_start_value(g_meter, indix, value);
    lv_meter_set_indicator_end_value(g_meter, indix, 50);
    lv_obj_invalidate(g_meter); // 刷新仪表盘
}

void update_meter_value2(int32_t value) {
    if (g_meter2 == NULL) return;
    lv_meter_set_indicator_value(g_meter2, indix2, value);
    lv_obj_invalidate(g_meter2); // 刷新仪表盘
}

void update_text_temp(
#ifdef IDF_VER
    float value
#else
    int value
#endif
) {
#ifdef IDF_VER
    lv_label_set_text_fmt(temp_label, "%2.1f°", value);
#else
    lv_label_set_text_fmt(temp_label, "%d°", (int)value);
#endif // IDF_VER
}

void update_text_humid(
#ifdef IDF_VER
    float value
#else
    int value
#endif
) {
#ifdef IDF_VER
    lv_label_set_text_fmt(humid, "%2.1f%%", value);
#else
    lv_label_set_text_fmt(humid, "%d%%", (int)value);
#endif
}

void panel1(lv_obj_t *scr) {
    page1 = lv_obj_create(scr);
    lv_obj_set_style_line_width(page1, 0, 0);
    lv_obj_set_style_border_width(page1, 0, 0);
    lv_obj_set_style_pad_all(page1, 0, 0);
    lv_obj_set_style_radius(page1, 0, 0);
    lv_obj_set_size(page1, LV_HOR_RES, LV_VER_RES);
    lv_obj_center(page1);

    time_obj = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(time_obj, &number_16px, LV_STATE_DEFAULT);
    update_time(0, 0, 0);
    lv_obj_set_pos(time_obj, 40, 5);
    bat_obj = lv_bar_create(lv_scr_act());
    bat_obj_test = lv_label_create(lv_scr_act());

    lv_obj_set_size(bat_obj, 65, 4);
    lv_obj_set_pos(bat_obj, 40, 25);
    lv_obj_set_pos(bat_obj_test, 120, 4);
    lv_label_set_text(bat_obj_test, "0");
    meter1(page1);
}


void panel2(lv_obj_t *scr) {
    page2 = lv_obj_create(scr);
    lv_obj_set_style_line_width(page2, 0, 0);
    lv_obj_set_style_border_width(page2, 0, 0);
    lv_obj_set_style_pad_all(page2, 0, 0);
    lv_obj_set_style_radius(page2, 0, 0);
    lv_obj_set_style_bg_color(page2, lv_color_hex(0xa6c1ee), 0);
    lv_obj_set_style_bg_grad_color(page2, lv_color_hex(0xfbc2eb), 0);
    lv_obj_set_style_bg_grad_dir(page2, LV_GRAD_DIR_HOR, 0);
    lv_obj_center(page2);
    lv_obj_set_size(page2, LV_HOR_RES, LV_VER_RES);

    temp_obj = lv_label_create(page2);
    lv_obj_align(temp_obj, LV_ALIGN_CENTER, -75, 20);

    weather_ch_obj = lv_label_create(page2);
    lv_obj_set_style_text_font(weather_ch_obj, &weather_chinese_24px, LV_STATE_DEFAULT);
    lv_obj_align(weather_ch_obj, LV_ALIGN_CENTER, -80, 70);

    // 创建右边那一部分

    lv_obj_t *feels_label = lv_label_create(page2);
    lv_obj_set_style_text_font(feels_label, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(feels_label, COLOR_LITTLE_BLACK, 0);
    lv_obj_set_pos(feels_label, 200, 30);
    lv_label_set_text(feels_label, "feels");

    lv_obj_t *humidityN_obj_label = lv_label_create(page2);
    lv_obj_set_style_text_font(humidityN_obj_label, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(humidityN_obj_label, COLOR_LITTLE_BLACK, 0);
    lv_obj_set_pos(humidityN_obj_label, 200, 30 + 44);
    lv_label_set_text(humidityN_obj_label, "humidity");


    lv_obj_t *vis_obj_label = lv_label_create(page2);
    lv_obj_set_style_text_font(vis_obj_label, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(vis_obj_label, COLOR_LITTLE_BLACK, 0);
    lv_obj_set_pos(vis_obj_label, 200, 30 + 44 + 44);
    lv_label_set_text(vis_obj_label, "vis");

    lv_obj_t *cloud_obj_label = lv_label_create(page2);
    lv_obj_set_style_text_font(cloud_obj_label, &lv_font_montserrat_14, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cloud_obj_label, COLOR_LITTLE_BLACK, 0);
    lv_obj_set_pos(cloud_obj_label, 200, 30 + 44 + 44 + 44);
    lv_label_set_text(cloud_obj_label, "cloud");

    feels_like_obj = lv_label_create(page2);
    lv_obj_set_style_text_font(feels_like_obj, &number_24px, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(feels_like_obj, lv_color_hex(0x36648B), 0);
    lv_obj_set_pos(feels_like_obj, 200, 50);
    // lv_label_set_text(feels_like_obj, "40°");


    //
    humidityN_obj = lv_label_create(page2);
    lv_obj_set_style_text_font(humidityN_obj, &number_24px, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(humidityN_obj, lv_color_hex(0x667eea), 0);
    lv_obj_set_pos(humidityN_obj, 200, 50 + 44);
    // lv_label_set_text(humidityN_obj, "40%");

    vis_obj = lv_label_create(page2);
    lv_obj_set_style_text_font(vis_obj, &number_24px, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(vis_obj, lv_color_hex(0x6CA6CD), 0);
    lv_obj_set_pos(vis_obj, 200, 50 + 44 + 44);
    // lv_label_set_text(vis_obj, "1000km");

    cloud_obj = lv_label_create(page2);
    lv_obj_set_style_text_font(cloud_obj, &number_24px, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(cloud_obj, lv_color_hex(0x696969), 0);
    lv_obj_set_pos(cloud_obj, 200, 50 + 44 + 44 + 44);
    // lv_label_set_text(cloud_obj, "10%");


    lv_obj_set_style_text_font(temp_obj, &number_40px, LV_STATE_DEFAULT);
    lv_color_t color = lv_color_hex(0x4F4F4F);
    lv_obj_set_style_text_color(temp_obj, color, 0);
    main_weather = lv_img_create(page2);
    lv_obj_set_pos(main_weather, 40, 30);


    static lv_point_t line_points[] = {{320 / 2, 40}, {320 / 2, 200}};
    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 2);
    lv_style_set_line_color(&style_line, lv_color_hex(0xeeeeee));
    lv_style_set_line_rounded(&style_line, true);
    lv_obj_t *line1;
    line1 = lv_line_create(page2);
    lv_line_set_points(line1, line_points, 2); /*Set the points*/
    lv_obj_add_style(line1, &style_line, 0);
    // lv_obj_center(line1);
}

static void wifi_switch_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
        if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
#ifdef IDF_VER
            task_conn(NULL);
            ESP_LOGI("PANEL", "started wifi");
#endif
        } else {
#ifdef IDF_VER
            esp_wifi_stop();
            esp_wifi_deinit();
            set_wifi_conn(0);
            ESP_LOGI("PANEL", "deinit wifi");
#endif
        }
    }
}

static void display_switch_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
        if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
#ifdef IDF_VER
            gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, 0);
            is_diplay = 1;
#endif
        } else {
#ifdef IDF_VER
            gpio_set_level(EXAMPLE_PIN_NUM_BK_LIGHT, 1);
            is_diplay = 0;
#endif
        }
    }
}

static void sync_time_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
#ifdef IDF_VER
        if (esp_sntp_enabled()) {
            return;
        }
        esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
        esp_sntp_setservername(0, "ntp1.aliyun.com");
        esp_sntp_init();
        esp_sntp_set_sync_status(SNTP_SYNC_STATUS_COMPLETED);
#endif
    }
}

static void sync_weather_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
#ifdef IDF_VER
        tcp_client2();
#endif
    }
}

void panel3(lv_obj_t *scr) {
    page3 = lv_obj_create(scr);
    lv_obj_set_style_line_width(page3, 0, 0);
    lv_obj_set_style_border_width(page3, 0, 0);
    // lv_obj_set_style_pad_all(page3, 0, 0);
    lv_obj_set_style_radius(page3, 0, 0);
    lv_obj_align(page3, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_size(page3, LV_HOR_RES, LV_VER_RES);
    lv_obj_t *sw;

    lv_obj_t *wifi_label = lv_label_create(page3);
    lv_obj_set_style_text_font(wifi_label, &lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(wifi_label, lv_color_hex(0x4F4F4F), 0);
    lv_label_set_text(wifi_label, "Wi-Fi");
    sw = lv_switch_create(page3);
    lv_obj_align(wifi_label, LV_ALIGN_LEFT_MID, 45, -60);
    lv_obj_align(sw, LV_ALIGN_LEFT_MID, 45, -30);

    if (
#ifdef IDF_VER
        get_wifi_conn()
#else
        1
#endif
    ) {
        lv_obj_add_state(sw, LV_STATE_CHECKED);
    }
    lv_obj_add_event_cb(sw, wifi_switch_event_handler, LV_EVENT_ALL, NULL);

    lv_obj_t *label;

    lv_obj_t *btn1 = lv_btn_create(page3);
    lv_obj_add_event_cb(btn1, sync_time_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn1, LV_ALIGN_RIGHT_MID, -45, -30);

    label = lv_label_create(btn1);
    lv_label_set_text(label, "Sync Time");
    lv_obj_center(label);

    lv_obj_t *label_weather;

    lv_obj_t *btn2 = lv_btn_create(page3);
    lv_obj_add_event_cb(btn2, sync_weather_event_handler, LV_EVENT_ALL, NULL);
    lv_obj_align(btn2, LV_ALIGN_RIGHT_MID, -20, 45);

    label_weather = lv_label_create(btn2);
    lv_label_set_text(label_weather, "Sync Weather");
    lv_obj_center(label_weather);


    lv_obj_t *display_label = lv_label_create(page3);
    lv_obj_set_style_text_font(display_label, &lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(display_label, lv_color_hex(0x4F4F4F), 0);
    lv_label_set_text(display_label, "Display");
    display_sw = lv_switch_create(page3);
    lv_obj_align(display_label, LV_ALIGN_LEFT_MID, 45, 15);
    lv_obj_align(display_sw, LV_ALIGN_LEFT_MID, 45, 45);
    if (
#ifdef IDF_VER
        !gpio_get_level(EXAMPLE_PIN_NUM_BK_LIGHT)
#else
        1
#endif
    ) {
        lv_obj_add_state(display_sw, LV_STATE_CHECKED);
        is_diplay = 1;
    }
    lv_obj_add_event_cb(display_sw, display_switch_event_handler, LV_EVENT_ALL, NULL);


    lv_group_add_obj(group, sw);
    lv_group_add_obj(group, btn1);
    lv_group_add_obj(group, display_sw);
    lv_group_add_obj(group, btn2);
}


void update_time(int h, int m, int s) {
    lv_label_set_text_fmt(time_obj, "%02d:%02d:%02d", h, m, s);
}

void update_bat(int b) {
    lv_bar_set_value(bat_obj, b, LV_ANIM_OFF);
    lv_label_set_text_fmt(bat_obj_test, "%d", b);
}

void set_weather(char *data) {
    cJSON *obj = cJSON_Parse(data);
    weather_obj.temp = cJSON_GetObjectItem(obj, "temp");
    weather_obj.text_icon = cJSON_GetObjectItem(obj, "text_icon");
    weather_obj.feels_like = cJSON_GetObjectItem(obj, "feelsLike");
    weather_obj.text = cJSON_GetObjectItem(obj, "text");
    weather_obj.humidity = cJSON_GetObjectItem(obj, "humidity");
    weather_obj.vis = cJSON_GetObjectItem(obj, "vis");
    weather_obj.cloud = cJSON_GetObjectItem(obj, "cloud");


    // char* icon_text[6];
    // memset(icon_text, 0, sizeof(icon_text));
    // unicode_to_utf8(weather_obj.icon->valueint, icon_text);
    // lv_label_set_text(icon, icon_text);
    // lv_label_set_text_fmt(temp, "%d", weather_obj.temp->valueint);

    lv_img_set_src(main_weather, get_weather_img_from_text(weather_obj.text_icon->valueint));
    lv_label_set_text_fmt(temp_obj, "%d°", weather_obj.temp->valueint);
    lv_label_set_text(weather_ch_obj, weather_obj.text->valuestring);
    lv_label_set_text_fmt(feels_like_obj, "%d°", weather_obj.feels_like->valueint);
    lv_label_set_text_fmt(humidityN_obj, "%d%%", weather_obj.humidity->valueint);
    lv_label_set_text_fmt(vis_obj, "%dkm", weather_obj.vis->valueint);
    lv_label_set_text_fmt(cloud_obj, "%d%%", weather_obj.cloud->valueint);

    // free(obj);
    // free(data);
}

void switch_panel() {
    if (point == 0) {
        lv_obj_add_flag(page1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(page3, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(page2, LV_OBJ_FLAG_HIDDEN);
        point = 1;
    } else if (point == 1) {
        lv_obj_add_flag(page1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(page2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(page3, LV_OBJ_FLAG_HIDDEN);
        point = 2;
    } else {
        lv_obj_add_flag(page3, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(page2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(page1, LV_OBJ_FLAG_HIDDEN);
        point = 0;
    }
}

void unicode_to_utf8(unsigned int codepoint, char *out) {
    if (codepoint <= 0x7F) {
        // 1-byte sequence
        *out++ = (char) (codepoint & 0xFF);
        *out = '\0';
    } else if (codepoint <= 0x7FF) {
        // 2-byte sequence
        *out++ = (char) (0xC0 | ((codepoint >> 6) & 0x1F));
        *out++ = (char) (0x80 | (codepoint & 0x3F));
        *out = '\0';
    } else if (codepoint <= 0xFFFF) {
        // 3-byte sequence
        *out++ = (char) (0xE0 | ((codepoint >> 12) & 0x0F));
        *out++ = (char) (0x80 | ((codepoint >> 6) & 0x3F));
        *out++ = (char) (0x80 | (codepoint & 0x3F));
        *out = '\0';
    } else if (codepoint <= 0x1FFFFF) {
        // 4-byte sequence
        *out++ = (char) (0xF0 | ((codepoint >> 18) & 0x07));
        *out++ = (char) (0x80 | ((codepoint >> 12) & 0x3F));
        *out++ = (char) (0x80 | ((codepoint >> 6) & 0x3F));
        *out++ = (char) (0x80 | (codepoint & 0x3F));
        *out = '\0';
    }
}

lv_img_dsc_t *cal_thi(float t, float h) {
    float a = (1.8 * t + 32) - (0.55 - 0.55 * h * 0.01) * (1.8 * t - 26);
    if (a < 40) {
        return &g626;
    } else if (a >= 40 && a < 55) {
        return &g600;
    } else if (a >= 55 && a < 70) {
        return &g36;
    } else if (a >= 70 && a < 75) {
        return &g110;
    } else if (a >= 75) {
        return &g616;
    }
    return &g626;
}

void update_emoji(float t, float h) {
    lv_img_dsc_t *a = cal_thi(t, h);
    lv_img_set_src(emoji_obj, a);
}

lv_img_dsc_t *get_weather_img_from_text(int c) {
    return weather_mapping_obj[c];
}

void shutdown_lcd() {
    if (is_diplay == 1) {
#ifdef IDF_VER
        gpio_set_level(2, 1);
#endif

        is_diplay = 0;
        lv_obj_clear_state(display_sw, LV_STATE_CHECKED);
    } else {
#ifdef IDF_VER
        gpio_set_level(2, 0);
#endif
        is_diplay = 1;
        lv_obj_add_state(display_sw, LV_STATE_CHECKED);
    }
}
