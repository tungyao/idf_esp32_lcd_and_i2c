//
// Created by Administrator on 2024/7/15.
//

#include "sto.h"

#include <esp_log.h>

void sto_init() {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}


void store_data(char *key, char *data) {
    nvs_handle_t my_handle;
    esp_err_t err;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK) {
        ESP_LOGE("STORE", "Error (%s) opening NVS handle!", esp_err_to_name(err));
    } else {
        ESP_LOGE("STORE", "Updating restart counter in NVS ... ");
        err = nvs_set_str(my_handle, key, data);
        ESP_LOGE("STORE", "%s", (err != ESP_OK) ? "Failed!" : "Done");
        // Commit written value.
        // After setting any values, nvs_commit() must be called to ensure changes are written
        // to flash storage. Implementations may write to storage at other times,
        // but this is not guaranteed.
        ESP_LOGE("STORE", "Committing updates in NVS ... ");
        err = nvs_commit(my_handle);
        ESP_LOGE("STORE", "%s", (err != ESP_OK) ? "Failed!" : "Done");

        // Close
        nvs_close(my_handle);
    }
}

uint8_t read_data(const char* key, char* out_value, size_t* length) {
    nvs_handle_t my_handle;
    esp_err_t ret;

    // 打开NVS命名空间
    ret = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (ret != ESP_OK) {
        return 0;
    }

    // 读取字符串
    ret = nvs_get_str(my_handle, key, out_value, length);
    if (ret != ESP_OK) {
        // 错误处理
        return 0;
    }

    // 关闭NVS句柄
    nvs_close(my_handle);
    return 1;
}