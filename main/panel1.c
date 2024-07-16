#include "panel1.h"


void meter1(lv_obj_t* scr)
{
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

void meter2(lv_obj_t* scr)
{
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


void update_meter_value(int32_t value)
{
    if (g_meter == NULL) return;
    lv_meter_set_indicator_value(g_meter, indix, value);
    lv_obj_invalidate(g_meter); // 刷新仪表盘
}

void update_meter_value2(int32_t value)
{
    if (g_meter2 == NULL) return;
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

void panel1(lv_obj_t* scr)
{
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

void panel2(lv_obj_t* scr)
{
    page2 = lv_obj_create(scr);
    lv_obj_set_style_line_width(page2, 0, 0);
    lv_obj_set_style_border_width(page2, 0, 0);
    lv_obj_set_style_pad_all(page2, 0, 0);
    lv_obj_set_style_radius(page2, 0, 0);
    lv_obj_align(page2, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_size(page2, LV_HOR_RES, LV_VER_RES);


    lv_obj_t* label;

    lv_obj_t* btn1 = lv_btn_create(page2);
    lv_obj_align(btn1, LV_ALIGN_CENTER, 0, -40);

    label = lv_label_create(btn1);
}

void switch_panel()
{
    if (point == 0)
    {
        lv_obj_add_flag(page1, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(page2, LV_OBJ_FLAG_HIDDEN);
        point = 1;
    }
    else
    {
        lv_obj_add_flag(page2, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(page1, LV_OBJ_FLAG_HIDDEN);
        point = 0;
    }
}
