#pragma once

#ifdef IDF_VER
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif

static lv_obj_t* temp_label;
static lv_obj_t* humid;


static lv_obj_t* g_meter = NULL;
static lv_meter_scale_t* scale1 = NULL;
static lv_meter_indicator_t* indix;


static lv_obj_t* g_meter2 = NULL;
static lv_meter_scale_t* scale2 = NULL;
static lv_meter_indicator_t* indix2;



void panel1(lv_obj_t* scr);
void panel2(lv_obj_t* scr);
void update_meter_value(int32_t value);
void update_meter_value2(int32_t value);
void update_text_value(float value);
void update_text_value2(float value);
