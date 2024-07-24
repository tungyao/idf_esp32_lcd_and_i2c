/*
 * PROJECT:   LVGL PC Simulator using Visual Studio
 * FILE:      LVGL.Simulator.cpp
 * PURPOSE:   Implementation for LVGL ported to Windows Desktop
 *
 * LICENSE:   The MIT License
 *
 * DEVELOPER: Mouri_Naruto (Mouri_Naruto AT Outlook.com)
 */

#include <Windows.h>

#include "resource.h"
#if _MSC_VER >= 1200
// Disable compilation warnings.
#pragma warning(push)
// nonstandard extension used : bit field types other than int
#pragma warning(disable:4214)
// 'conversion' conversion from 'type1' to 'type2', possible loss of data
#pragma warning(disable:4244)
#endif

#include "lvgl/lvgl.h"
#include "lvgl/examples/lv_examples.h"
#include "lvgl/demos/lv_demos.h"
#include "lv_drivers/win32drv/win32drv.h"
#include "lvgl/src/extra/widgets/meter/lv_meter.h"

#if _MSC_VER >= 1200
// Restore compilation warnings.
#pragma warning(pop)
#endif

#include <stdio.h>

bool single_display_mode_initialization()
{
    if (!lv_win32_init(
        GetModuleHandleW(NULL),
        SW_SHOW,
        320,
        240,
        LoadIconW(GetModuleHandleW(NULL), MAKEINTRESOURCE(IDI_LVGL))))
    {
        return false;
    }

    lv_win32_add_all_input_devices_to_group(NULL);

    return true;
}

#include <process.h>

HANDLE g_window_mutex = NULL;
bool g_initialization_status = false;

#define LVGL_SIMULATOR_MAXIMUM_DISPLAYS 16
HWND g_display_window_handles[LVGL_SIMULATOR_MAXIMUM_DISPLAYS];

unsigned int __stdcall lv_win32_window_thread_entrypoint(
    void* raw_parameter)
{
    size_t display_id = *(size_t*)(raw_parameter);

    HINSTANCE instance_handle = GetModuleHandleW(NULL);
    int show_window_mode = SW_SHOW;
    HICON icon_handle = LoadIconW(instance_handle, MAKEINTRESOURCE(IDI_LVGL));
    lv_coord_t hor_res = 320;
    lv_coord_t ver_res = 240;

    wchar_t window_title[256];
    memset(window_title, 0, sizeof(window_title));
    _snwprintf(
        window_title,
        256,
        L"LVGL Simulator for Windows Desktop (Display %d)",
        display_id);

    g_display_window_handles[display_id] = lv_win32_create_display_window(
        window_title,
        hor_res,
        ver_res,
        instance_handle,
        icon_handle,
        show_window_mode);
    if (!g_display_window_handles[display_id])
    {
        return 0;
    }

    g_initialization_status = true;

    SetEvent(g_window_mutex);

    MSG message;
    while (GetMessageW(&message, NULL, 0, 0))
    {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    lv_win32_quit_signal = true;

    return 0;
}

bool multiple_display_mode_initialization()
{
    if (!lv_win32_init_window_class())
    {
        return false;
    }

    for (size_t i = 0; i < LVGL_SIMULATOR_MAXIMUM_DISPLAYS; ++i)
    {
        g_initialization_status = false;

        g_window_mutex = CreateEventExW(NULL, NULL, 0, EVENT_ALL_ACCESS);

        _beginthreadex(
            NULL,
            0,
            lv_win32_window_thread_entrypoint,
            &i,
            0,
            NULL);

        WaitForSingleObjectEx(g_window_mutex, INFINITE, FALSE);

        CloseHandle(g_window_mutex);

        if (!g_initialization_status)
        {
            return false;
        }
    }

    lv_win32_window_context_t* context = (lv_win32_window_context_t*)(
        lv_win32_get_window_context(g_display_window_handles[0]));
    if (context)
    {
        lv_win32_pointer_device_object = context->mouse_device_object;
        lv_win32_keypad_device_object = context->keyboard_device_object;
        lv_win32_encoder_device_object = context->mousewheel_device_object;
    }

    lv_win32_add_all_input_devices_to_group(NULL);

    return true;
}

//static lv_obj_t* label;
//
//static void slider_event_cb(lv_event_t* e)
//{
//    lv_obj_t* slider = lv_event_get_target(e);
//
//    /*Refresh the text*/
//    lv_label_set_text_fmt(label, "%d", lv_slider_get_value(slider));
//    lv_obj_align_to(label, slider, LV_ALIGN_OUT_TOP_MID, 0, -15);    /*Align top of the slider*/
//}

static void set_value(void* indic, int32_t v)
{
    // lv_meter_set_indicator_value(meter, indic, v);
}


#include "panel1.h"
#include <iostream>
#include <random>


static lv_meter_indicator_t* indic = {};


// void lv_example_tabview_1(void)
// {
//     /*Create a Tab view object*/
//     lv_obj_t* tabview;
//     tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 50);
//
//     /*Add 3 tabs (the tabs are page (lv_page) and can be scrolled*/
//     lv_obj_t* tab1 = lv_tabview_add_tab(tabview, "Tab 1");
//     lv_obj_t* tab2 = lv_tabview_add_tab(tabview, "Tab 2");
//     lv_obj_t* tab3 = lv_tabview_add_tab(tabview, "Tab 3");
//
//     /*Add content to the tabs*/
//     lv_obj_t* label = lv_label_create(tab1);
//     lv_label_set_text(label, "This the first tab\n\n"
//         "If the content\n"
//         "of a tab\n"
//         "becomes too\n"
//         "longer\n"
//         "than the\n"
//         "container\n"
//         "then it\n"
//         "automatically\n"
//         "becomes\n"
//         "scrollable.\n"
//         "\n"
//         "\n"
//         "\n"
//         "Can you see it?");
//
//     label = lv_label_create(tab2);
//     lv_label_set_text(label, "Second tab");
//
//     label = lv_label_create(tab3);
//     lv_label_set_text(label, "Third tab");
//
//     lv_obj_scroll_to_view_recursive(label, LV_ANIM_ON);
//
// }

static void event_handler(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED)
    {
        LV_LOG_USER("Clicked");
        switch_panel();
    }
    else if (code == LV_EVENT_VALUE_CHANGED)
    {
        LV_LOG_USER("Toggled");
    }
}

int main()
{
    lv_init();

    if (!single_display_mode_initialization())
    {
        return -1;
    }

    lv_obj_t* label;


    lv_obj_t* page = lv_obj_create(lv_scr_act());
    lv_obj_set_size(page, LV_HOR_RES, LV_VER_RES); //设置到屏幕大小
    lv_obj_set_style_line_width(page, 0, 0);
    lv_obj_set_style_border_width(page, 0, 0);
    lv_obj_set_style_pad_all(page, 0, 0);
    char data[] =
        "{\"temp\":35,\"feelsLike\":38,\"icon\":61697,\"text\":\"多云\",\"text_icon\":0,\"wind360\":180,\"windDir\":\"南风\",\"windScale\":2,\"windSpeed\":6,\"humidity\":46,\"precip\":\"0.0\",\"pressure\":967,\"vis\":30,\"cloud\":91,\"dew\":23}";
    panel1(page);
    panel2(page);

    // switch_panel();
    // set_weather(data);
    // lv_obj_add_flag(page2, LV_OBJ_FLAG_HIDDEN);
    // lv_example_tabview_1();
    // panel1(tab1);
    // meter1(lv_scr_act());
    // panel2(lv_scr_act());
    // lv_meter_2(lv_scr_act());
    // ----------------------------------
    // Task handler loop
    // ----------------------------------

    while (!lv_win32_quit_signal)
    {
        std::mt19937 generator(std::random_device{}());

        // 分配一个分布范围
        std::uniform_int_distribution<int> distribution(-10, 50); // 0 到 99

        // 生成随机数
        switch_panel();
        int random_number = distribution(generator);
        update_meter_value(random_number);
        update_text_temp(random_number);
        update_text_humid(random_number);
        update_emoji(random_number, random_number);
        set_weather(data);
        lv_task_handler();


        Sleep(1000);
        // switch_panel();
    }

    return 0;
}
