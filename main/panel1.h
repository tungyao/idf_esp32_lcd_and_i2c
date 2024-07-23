﻿#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#ifdef IDF_VER
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
#include <cJSON.h>

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

void panel1(lv_obj_t* scr);

void panel2(lv_obj_t* scr);

void switch_panel();

lv_img_dsc_t* cal_thi(float t, float h);

// 初始化字库
LV_FONT_DECLARE(weather_chinese)
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
LV_IMG_DECLARE(wi_wind)
LV_IMG_DECLARE(wi_windsock)
LV_IMG_DECLARE(wi_dust)


void set_weather(char* data);
static lv_obj_t* main_weather;
static lv_obj_t* emoji_obj;
void unicode_to_utf8(unsigned int codepoint, char* out);
void update_emoji(float t, float h);
// cjson
static lv_obj_t* temp_obj;
static lv_obj_t* weather_ch_obj;
static lv_obj_t* feels_like_obj;
static lv_obj_t* icon_obj;
static lv_obj_t* text_obj;
static lv_obj_t* wind360_obj;
static lv_obj_t* windDir_obj;
static lv_obj_t* windScale_obj;
static lv_obj_t* windSpeed_obj;
static lv_obj_t* humidityN_obj;
static lv_obj_t* precip_obj;
static lv_obj_t* pressure_obj;
static lv_obj_t* vis_obj;
static lv_obj_t* cloud_obj;
static lv_obj_t* dew_obj;
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
struct _weather_ovj
{
    cJSON* temp;
    cJSON* feels_like;
    cJSON* icon;
    cJSON* text;
    cJSON* wind360;
    cJSON* windDir;
    cJSON* windScale;
    cJSON* windSpeed;
    cJSON* humidity;
    cJSON* precip;
    cJSON* pressure;
    cJSON* vis;
    cJSON* cloud;
    cJSON* dew;
} static weather_obj;

static const lv_img_dsc_t* weather_mapping_obj[] = {
    &wi_clear_day,
    &wi_cloudy,
    &wi_drizzle,
    &wi_fog,
    &wi_hail,
    &wi_haze,
    &wi_mist,
    &wi_overcast,
    &wi_rain,
    &wi_sleet,
    &wi_snow,
    &wi_thunderstorms,
    &wi_thunderstorms_rain,
    &wi_wind,
    &wi_windsock,
    &wi_dust
};

lv_img_dsc_t* get_weather_img_from_text(int);
#ifdef __cplusplus
} /*extern "C"*/
#endif
