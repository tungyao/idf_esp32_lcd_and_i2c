#include "panel1.h"

#include <math.h>


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
    lv_obj_align(temp_label_t, LV_TEXT_ALIGN_CENTER, 0, 162);
    lv_label_set_text(temp_label_t, "Temperature");


    // 显示温度计图标
    LV_IMG_DECLARE(humid_icon)
    lv_obj_t *img1 = lv_img_create(scr);
    lv_img_set_src(img1, &humid_icon);
    lv_obj_set_pos(img1, 40, 190);
    lv_obj_set_size(img1, 20, 20);


    // 创建湿度数字
    humid = lv_label_create(scr);
    lv_obj_set_style_text_font(humid, &lv_font_montserrat_20, LV_STATE_DEFAULT);
    lv_obj_set_pos(humid, 65, 190);
    lv_label_set_text_fmt(humid, "%s", "90%");
    lv_obj_set_scrollbar_mode(scr, LV_SCROLLBAR_MODE_OFF);


    // 创建里面那层半圆点点
    g_meter2 = lv_meter_create(scr);
    lv_obj_remove_style(g_meter2, NULL, LV_PART_MAIN);
    lv_obj_remove_style(g_meter2, NULL, LV_PART_INDICATOR);
    lv_obj_align(g_meter2, LV_ALIGN_CENTER, 0, 50);
    lv_obj_set_size(g_meter2, 190, 190);
    scale2 = lv_meter_add_scale(g_meter2);
    lv_meter_set_scale_range(g_meter2, scale2, -10, 50, 180, 180);
    lv_meter_set_scale_ticks(g_meter2, scale2, 61, 2, 2, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(g_meter2, scale2, 30, 2, 5, lv_color_black(), 16);
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
    lv_meter_set_scale_ticks(g_meter, scale1, 41, 2, 25, lv_palette_main(LV_PALETTE_GREY));
    indix = lv_meter_add_scale_lines(g_meter, scale1, lv_palette_main(LV_PALETTE_GREEN),
                                     lv_palette_main(LV_PALETTE_RED),
                                     0, 0);
    lv_meter_set_indicator_start_value(g_meter, indix, -10);
    lv_meter_set_indicator_end_value(g_meter, indix, 50);
    lv_color_t my_color = lv_color_hex(0xFFFFFF);
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
    meter1(page1);
    // meter2(page1);
}

#include "cJSON.h"

const char *example =
        "{\"temp\":35,\"feelsLike\":38,\"icon\":61697,\"text\":\"多云\",\"wind360\":180,\"windDir\":\"南风\",\"windScale\":2,\"windSpeed\":6,\"humidity\":46,\"precip\":\"0.0\",\"pressure\":967,\"vis\":30,\"cloud\":91,\"dew\":23}";

void panel2(lv_obj_t *scr) {
    page2 = lv_obj_create(scr);
    lv_obj_set_style_line_width(page2, 0, 0);
    lv_obj_set_style_border_width(page2, 0, 0);
    lv_obj_set_style_pad_all(page2, 0, 0);
    lv_obj_set_style_radius(page2, 0, 0);
    lv_obj_align(page2, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_size(page2, LV_HOR_RES, LV_VER_RES);
    temp = lv_label_create(page2);
    feels_like = lv_label_create(page2);
    icon = lv_label_create(page2);
    text = lv_label_create(page2);
    wind360 = lv_label_create(page2);
    windDir = lv_label_create(page2);
    windScale = lv_label_create(page2);
    windSpeed = lv_label_create(page2);
    humidityN = lv_label_create(page2);
    precip = lv_label_create(page2);
    pressure = lv_label_create(page2);
    vis = lv_label_create(page2);
    cloud = lv_label_create(page2);
    dew = lv_label_create(page2);

    lv_obj_set_style_text_font(icon, &weather_icon, LV_STATE_DEFAULT);


    lv_obj_set_style_text_font(temp, &lv_font_montserrat_32, LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(feels_like, &lv_font_montserrat_32, LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(text, &weather_chinese, LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(humidityN, &weather_chinese, LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(pressure, &weather_chinese, LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(cloud, &weather_chinese, LV_STATE_DEFAULT);
}

void set_weather(char *data) {
    cJSON *obj = cJSON_Parse(data);
    weather_obj.temp = cJSON_GetObjectItem(obj, "temp");
    weather_obj.feels_like = cJSON_GetObjectItem(obj, "feelsLike");
    weather_obj.icon = cJSON_GetObjectItem(obj, "icon");
    weather_obj.text = cJSON_GetObjectItem(obj, "text");
    weather_obj.wind360 = cJSON_GetObjectItem(obj, "wind360");
    weather_obj.windDir = cJSON_GetObjectItem(obj, "windDir");
    weather_obj.windScale = cJSON_GetObjectItem(obj, "windScale");
    weather_obj.windSpeed = cJSON_GetObjectItem(obj, "windSpeed");
    weather_obj.humidity = cJSON_GetObjectItem(obj, "humidity");
    weather_obj.precip = cJSON_GetObjectItem(obj, "precip");
    weather_obj.pressure = cJSON_GetObjectItem(obj, "pressure");
    weather_obj.vis = cJSON_GetObjectItem(obj, "vis");
    weather_obj.cloud = cJSON_GetObjectItem(obj, "cloud");
    weather_obj.dew = cJSON_GetObjectItem(obj, "dew");


    char *icon_text[6];
    memset(icon_text, 0, sizeof(icon_text));
    unicode_to_utf8(weather_obj.icon->valueint, icon_text);
    lv_label_set_text(icon, icon_text);
    lv_label_set_text_fmt(temp, "%d", weather_obj.temp->valueint);


    // free(obj);
    // free(data);
}

void switch_panel() {
    if (point == 0) {
        lv_obj_add_flag(page1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(page2, LV_OBJ_FLAG_HIDDEN);
        point = 1;
    } else {
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
