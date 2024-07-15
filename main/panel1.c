#include "panel1.h"


void panel1(lv_obj_t* scr)
{
    temp_label = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(temp_label, &lv_font_montserrat_32, LV_STATE_DEFAULT);
    lv_obj_align(temp_label, LV_TEXT_ALIGN_CENTER, 10, 5);
    lv_label_set_text_fmt(temp_label, "%-5s:  22.22 deg", "temp");
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

void panel2(lv_obj_t* scr)
{
    humid = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(humid, &lv_font_montserrat_32, LV_STATE_DEFAULT);
    lv_obj_align(humid, LV_TEXT_ALIGN_CENTER, 10, 40);
    g_meter2 = lv_meter_create(scr);
    lv_obj_set_pos(g_meter2, 160, 80);
    lv_obj_set_size(g_meter2, 150, 150);
    scale2 = lv_meter_add_scale(g_meter2);
    lv_meter_set_scale_range(g_meter2, scale2, 0, 100, 300, 120);
    lv_meter_set_scale_ticks(g_meter2, scale2, 101, 2, 5, lv_palette_main(LV_PALETTE_GREY));
    lv_meter_set_scale_major_ticks(g_meter2, scale2, 10, 2, 12, lv_color_black(), 12);
    indix2 = lv_meter_add_needle_line(g_meter2, scale2, 3, lv_palette_main(LV_PALETTE_ORANGE), -10);

}


void update_meter_value(int32_t value)
{
    if (g_meter == NULL) return;

    // indix = lv_meter_add_needle_line(g_meter, scale1, 3,  lv_palette_main(LV_PALETTE_ORANGE), -10);
    lv_meter_set_indicator_value(g_meter, indix, value);
    lv_obj_invalidate(g_meter); // 刷新仪表盘
}

void update_meter_value2(int32_t value)
{
    if (g_meter2 == NULL) return;

    // indix = lv_meter_add_needle_line(g_meter, scale1, 3,  lv_palette_main(LV_PALETTE_ORANGE), -10);
    lv_meter_set_indicator_value(g_meter2, indix2, value);
    lv_obj_invalidate(g_meter2); // 刷新仪表盘
}

void update_text_value(float value)
{
#ifdef IDF_VER
    lv_label_set_text_fmt(temp_label, "%-5s:  %2.2f deg", "temp", value);
#else
    lv_label_set_text_fmt(temp_label, "%-5s:  %d deg", "temp", (int)value);
#endif // IDF_VER

}

void update_text_value2(float value)
{
#ifdef IDF_VER
    lv_label_set_text_fmt(humid, "%-5s: %2.2f  %%", "humid", value);
#else
    lv_label_set_text_fmt(humid, "%-5s: %d  %%", "humid", (int)value);
#endif
}

