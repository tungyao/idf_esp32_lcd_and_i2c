#pragma once
#ifdef __cplusplus
extern "C" {
#endif
#ifdef IDF_VER
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif
#include <cJSON.h>

static lv_obj_t *temp_label;
static lv_obj_t *humid;


static lv_obj_t *g_meter;
static lv_meter_scale_t *scale1 = NULL;
static lv_meter_indicator_t *indix;


static lv_obj_t *g_meter2;
static lv_meter_scale_t *scale2 = NULL;
static lv_meter_indicator_t *indix2;


void meter1(lv_obj_t *scr);

void meter2(lv_obj_t *scr);

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

static lv_obj_t *page1 = NULL;
static lv_obj_t *page2 = NULL;

void panel1(lv_obj_t *scr);

void panel2(lv_obj_t *scr);

void switch_panel();


// 初始化字库
LV_FONT_DECLARE(weather_chinese)
LV_FONT_DECLARE(weather_icon)
LV_FONT_DECLARE(number_48px)
LV_FONT_DECLARE(number_40px)

void set_weather(char *data);

void unicode_to_utf8(unsigned int codepoint, char *out);

// cjson
static lv_obj_t *temp;
static lv_obj_t *feels_like;
static lv_obj_t *icon;
static lv_obj_t *text;
static lv_obj_t *wind360;
static lv_obj_t *windDir;
static lv_obj_t *windScale;
static lv_obj_t *windSpeed;
static lv_obj_t *humidityN;
static lv_obj_t *precip;
static lv_obj_t *pressure;
static lv_obj_t *vis;
static lv_obj_t *cloud;
static lv_obj_t *dew;
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
struct _weather_ovj {
    cJSON *temp;
    cJSON *feels_like;
    cJSON *icon;
    cJSON *text;
    cJSON *wind360;
    cJSON *windDir;
    cJSON *windScale;
    cJSON *windSpeed;
    cJSON *humidity;
    cJSON *precip;
    cJSON *pressure;
    cJSON *vis;
    cJSON *cloud;
    cJSON *dew;
} static weather_obj;
#ifdef __cplusplus
} /*extern "C"*/
#endif
