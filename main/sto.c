//
// Created by Administrator on 2024/7/15.
//

#include "sto.h"

void sto_init() {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}


void store_data(char *key,char *data) {
    esp_err_t err;

    // 打开命名空间
    nvs_handle my_nvs_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_nvs_handle);
    if (err != ESP_OK) {
        return;
    }

    // 设置数据
    // const char* data = "Hello, NVS!";
    err = nvs_set_str(my_nvs_handle, key, data);
    if (err != ESP_OK) {
        return;
    }

    // 提交更改
    err = nvs_commit(my_nvs_handle);
    free(data);

    if (err != ESP_OK) {
        return;
    }
    // 关闭命名空间
    nvs_close(my_nvs_handle);
}

char *read_data(char *key) {
    esp_err_t err;

    // 打开命名空间
    nvs_handle my_nvs_handle;
    err = nvs_open("storage", NVS_READONLY, &my_nvs_handle);
    if (err != ESP_OK) {
        return NULL;
    }

    size_t required_size;
    err = nvs_get_str(my_nvs_handle, key, NULL, &required_size);
    free(key);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return NULL;
    } else if (err != ESP_OK) {
        return NULL;
    }

    char *data = (char *) malloc(required_size);
    if (data == NULL) {
        return NULL;
    }

    err = nvs_get_str(my_nvs_handle, key, data, &required_size);
    if (err != ESP_OK) {
        free(data);
        return NULL;
    }
    nvs_close(my_nvs_handle);
    return data;
}