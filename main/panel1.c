#include "panel1.h"


void meter1(lv_obj_t *scr) {
    temp_label = lv_label_create(scr);
    lv_obj_set_style_text_font(temp_label, &lv_font_montserrat_32, LV_STATE_DEFAULT);
    lv_obj_align(temp_label, LV_TEXT_ALIGN_CENTER, 10, 5);
    lv_label_set_text_fmt(temp_label, "%-5s:  %s", "temp", "loading");
    g_meter = lv_meter_create(scr);
    lv_obj_set_pos(g_meter, 10, 80);
    lv_obj_set_size(g_meter, 150, 150);
    scale1 = lv_meter_add_scale(g_meter);
    lv_meter_set_scale_range(g_meter, scale1, 0, 50, 270, 135);
    lv_meter_set_scale_ticks(g_meter, scale1, 51, 2, 10, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(g_meter, scale1, 10, 2, 15, lv_color_black(), 15);
    indix = lv_meter_add_arc(g_meter, scale1, 3, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_meter_set_indicator_start_value(g_meter, indix, 0);
    lv_meter_set_indicator_end_value(g_meter, indix, 25);
    indix = lv_meter_add_scale_lines(g_meter, scale1, lv_palette_main(LV_PALETTE_BLUE),
                                     lv_palette_main(LV_PALETTE_BLUE),
                                     false, 0);
    lv_meter_set_indicator_start_value(g_meter, indix, 0);
    lv_meter_set_indicator_end_value(g_meter, indix, 25);
    indix = lv_meter_add_arc(g_meter, scale1, 3, lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(g_meter, indix, 35);
    lv_meter_set_indicator_end_value(g_meter, indix, 60);
    indix = lv_meter_add_scale_lines(g_meter, scale1, lv_palette_main(LV_PALETTE_RED), lv_palette_main(LV_PALETTE_RED),
                                     false, 0);
    lv_meter_set_indicator_start_value(g_meter, indix, 35);
    lv_meter_set_indicator_end_value(g_meter, indix, 60);
    indix = lv_meter_add_needle_line(g_meter, scale1, 3, lv_palette_main(LV_PALETTE_ORANGE), -10);
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
    lv_meter_set_indicator_value(g_meter, indix, value);
    lv_obj_invalidate(g_meter); // 刷新仪表盘
}

void update_meter_value2(int32_t value) {
    if (g_meter2 == NULL) return;
    lv_meter_set_indicator_value(g_meter2, indix2, value);
    lv_obj_invalidate(g_meter2); // 刷新仪表盘
}

void update_text_value(float value) {
#ifdef IDF_VER
    lv_label_set_text_fmt(temp_label, "%-5s:  %2.2f deg", "temp", value);
#else
    lv_label_set_text_fmt(temp_label, "%-5s:  %d deg", "temp", (int)value);
#endif // IDF_VER
}

void update_text_value2(float value) {
#ifdef IDF_VER
    lv_label_set_text_fmt(humid, "%-5s: %2.2f  %%", "humid", value);
#else
    lv_label_set_text_fmt(humid, "%-5s: %d  %%", "humid", (int)value);
#endif
}

void panel1(lv_obj_t *scr) {
    page1 = lv_obj_create(scr);
    lv_obj_set_style_line_width(page1, 0, 0);
    lv_obj_set_style_border_width(page1, 0, 0);
    lv_obj_set_style_pad_all(page1, 0, 0);
    lv_obj_set_style_radius(page1, 0, 0);

    lv_obj_align(page1, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_size(page1, LV_HOR_RES, LV_VER_RES);

    meter1(page1);
    meter2(page1);
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


    free(obj);
    free(data);
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
