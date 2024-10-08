﻿#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#ifdef IDF_VER
#include "lvgl.h"
#include <driver/gpio.h>
#else
#include "lvgl/lvgl.h"
#endif
#include <cJSON.h>
#define LCD_HOST  SPI2_HOST
#define EXAMPLE_LCD_PIXEL_CLOCK_HZ     (20 * 1000 * 1000)
#define EXAMPLE_LCD_BK_LIGHT_ON_LEVEL  1
#define EXAMPLE_LCD_BK_LIGHT_OFF_LEVEL !EXAMPLE_LCD_BK_LIGHT_ON_LEVEL
#define EXAMPLE_PIN_NUM_SCLK           6
#define EXAMPLE_PIN_NUM_MOSI           4
#define EXAMPLE_PIN_NUM_MISO           -1
#define EXAMPLE_PIN_NUM_LCD_DC         3
#define EXAMPLE_PIN_NUM_LCD_RST        -1
#define EXAMPLE_PIN_NUM_LCD_CS         5
#define EXAMPLE_PIN_NUM_BK_LIGHT       2
#define EXAMPLE_PIN_NUM_TOUCH_CS       -1
#define EXAMPLE_LCD_H_RES               320
#define EXAMPLE_LCD_V_RES              240
#define EXAMPLE_LCD_CMD_BITS           8
#define EXAMPLE_LCD_PARAM_BITS         8

#define EXAMPLE_LVGL_TICK_PERIOD_MS    2

static lv_indev_t* btn_index = NULL;
static lv_group_t* group;

void init_lcd();

static lv_disp_draw_buf_t disp_buf; // contains internal graphic buffer(s) called draw buffer(s)
static lv_disp_drv_t disp_drv; // contains callback functions
static lv_indev_drv_t indev_drv_key;

static lv_disp_t* disp;

lv_disp_t* get_disp();

static lv_obj_t* temp_label;
static lv_obj_t* humid;


static lv_obj_t* g_meter;
static lv_meter_scale_t* scale1 = NULL;
static lv_meter_indicator_t* indix;


static lv_obj_t* g_meter2;
static lv_meter_scale_t* scale2 = NULL;
static lv_meter_indicator_t* indix2;


void meter1(lv_obj_t* scr);

void meter2(lv_obj_t* scr);

void update_meter_value(int32_t value);

void update_meter_value2(int32_t value);

void update_text_temp(
#ifdef IDF_VER
    float value
#else
    int value
#endif
);

void update_text_humid(
#ifdef IDF_VER
    float value
#else
    int value
#endif
);

static int point = 0;

static lv_obj_t* page1 = NULL;
static lv_obj_t* page2 = NULL;
static lv_obj_t* page3 = NULL;
static void next_panel_cb(lv_event_t* e);

void init_sim();

void panel1(lv_obj_t* scr);

void panel2(lv_obj_t* scr);

void panel3(lv_obj_t* scr);

void switch_panel();

lv_img_dsc_t* cal_thi(float t, float h);

// 初始化字库
LV_FONT_DECLARE(weather_chinese)
LV_FONT_DECLARE(number_16px)
LV_FONT_DECLARE(number_24px)
LV_FONT_DECLARE(number_48px)
LV_FONT_DECLARE(number_40px)
LV_FONT_DECLARE(emoji)
LV_FONT_DECLARE(weather_chinese_24px)

// 初始化图片
LV_IMG_DECLARE(g616)
LV_IMG_DECLARE(g110)
LV_IMG_DECLARE(g36)
LV_IMG_DECLARE(g600)
LV_IMG_DECLARE(g626)
LV_IMG_DECLARE(wi_clear_day)
LV_IMG_DECLARE(wi_partly_cloudy_day_rain)
LV_IMG_DECLARE(wi_cloudy)
LV_IMG_DECLARE(wi_drizzle)
LV_IMG_DECLARE(wi_fog)
LV_IMG_DECLARE(wi_hail)
LV_IMG_DECLARE(wi_haze)
LV_IMG_DECLARE(wi_mist)
LV_IMG_DECLARE(wi_overcast)
LV_IMG_DECLARE(wi_rain)
LV_IMG_DECLARE(wi_sleet)
LV_IMG_DECLARE(wi_snow)
LV_IMG_DECLARE(wi_thunderstorms)
LV_IMG_DECLARE(wi_thunderstorms_rain)
LV_IMG_DECLARE(wi_dust)
LV_IMG_DECLARE(wi_hurricane)

static uint8_t is_diplay = 1;

void shutdown_lcd();

void set_weather(char* data);

static lv_obj_t* main_weather;
static lv_obj_t* emoji_obj;

void unicode_to_utf8(unsigned int codepoint, char* out);

void update_emoji(float t, float h);

void update_time(int h, int m, int s);

void update_bat(int b);


// cjson
static lv_obj_t* led_obj;
static lv_obj_t* time_obj;
static lv_obj_t* bat_obj;
static lv_obj_t* bat_obj_test;
static lv_obj_t* temp_obj;
static lv_obj_t* weather_ch_obj;
static lv_obj_t* feels_like_obj;
static lv_obj_t* text_obj;
static lv_obj_t* humidityN_obj;
static lv_obj_t* vis_obj;
static lv_obj_t* cloud_obj;
static lv_obj_t* display_sw;
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
struct _weather_ovj
{
    cJSON* temp;
    cJSON* text_icon;
    cJSON* feels_like;
    cJSON* text;
    cJSON* humidity;
    cJSON* vis;
    cJSON* cloud;
} static weather_obj;

static const lv_img_dsc_t* weather_mapping_obj[] = {
    &wi_clear_day, //0
    &wi_cloudy, //1
    &wi_partly_cloudy_day_rain, //2
    &wi_drizzle, //3
    &wi_fog, //4
    &wi_hail, //5
    &wi_haze, //6
    &wi_mist, //7
    &wi_overcast, //8
    &wi_rain, //9
    &wi_sleet, //10
    &wi_snow, //11
    &wi_thunderstorms, //12
    &wi_thunderstorms_rain, //13
    &wi_dust, //16
    &wi_hurricane //17
};


#define COLOR_LITTLE_BLACK lv_color_hex(0x8B8989)

lv_img_dsc_t* get_weather_img_from_text(int);
#ifdef __cplusplus
} /*extern "C"*/
#endif
