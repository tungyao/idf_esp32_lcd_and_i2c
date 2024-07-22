#include <stdio.h>
#include "conn.h"

#include <esp_timer.h>
#include <strings.h>
#include <driver/gpio.h>
#include <driver/uart.h>
#include <freertos/projdefs.h>
#include <sys/socket.h>

#include "panel1.h"
#include "sto.h"
#include "tcp_client.h"

static void IRAM_ATTR key_isr_handler(void *arg) {
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(key_queue, &gpio_num, NULL);
}

void conn_keys_init() {
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_NEGEDGE, // Disable interrupt
        .mode = GPIO_MODE_INPUT, // Set as Input
        .pin_bit_mask = (1ULL << IO19), // Bitmask
        .pull_down_en = 1, // Enable pull-up
        .pull_up_en = 0, // Enable pull-up
    };
    gpio_config(&io_conf);

    gpio_config_t io_conf2 = {
        .intr_type = GPIO_INTR_NEGEDGE, // Disable interrupt
        .mode = GPIO_MODE_INPUT, // Set as Input
        .pin_bit_mask = (1ULL << IO18), // Bitmask
        .pull_down_en = 1, // Enable pull-up
        .pull_up_en = 0, // Enable pull-up
    };
    gpio_config(&io_conf2);

    key_queue = xQueueCreate(1, sizeof(uint32_t));
    gpio_intr_enable(IO19);
    gpio_intr_enable(IO18);

    gpio_install_isr_service(0);
    // gpio_isr_register(IO19, key_isr_handler, NULL, 0, 0);
    gpio_isr_handler_add(IO19, key_isr_handler, (void *) IO19);
    gpio_isr_handler_add(IO18, key_isr_handler, (void *) IO18);
}

void change_input_mode() {
    if (input_mode == 0) {
        input_mode = 1;
    } else {
        input_mode = 0;
    }
}

uint8_t get_input_mode() {
    return input_mode;
}

void listen_config_key() {
    input_mode = 0;
    uint32_t gpio_num;
    // Set UART receive callback function
    while (1) {
        //
        if (xQueueReceive(key_queue, &gpio_num, portMAX_DELAY)) {
            if (gpio_num == IO19) {
                // vTaskDelay(50 / portTICK_PERIOD_MS);

                buttonPressed = true;
                uint64_t currentTime = esp_timer_get_time();
                if ((currentTime - lastButtonPressTime) < (DOUBLE_CLICK_TIME * 1000)) {
                    // 检测到双击
                    buttonPressCount++;
                    if (buttonPressCount == 2) {
                        buttonPressCount = 0; // 重置点击计数
                        change_input_mode();
                        // tcp_client2();
                        ESP_LOGI("RX_TASK_TAG", "double click IO19");
                        // xTaskCreate(listen_uart, "uart", 4096,NULL, 24,NULL);
                    }
                } else {
                    buttonPressCount = 1; // 重置点击计数
                    switch_panel();
                }
                lastButtonPressTime = currentTime; // 更新最后按下时间
            } else {
                buttonPressed = false;
            }
            if (gpio_num == IO18) {
                switch_panel();
            }
        }
    }
}

// 使用uart和esp32交互
void uart_event(char *data, int length) {
    char newdata[length - 3];
    for (int i = 3; i < length; ++i) {
        newdata[i - 3] = data[i];
    }
    ESP_LOGI("EVENT", "'%s'", newdata);

    // 放入wifi账号和密码
    if (data[0] == 'p' && data[1] == 'w' && data[2] == 'd') {
        // 设置wifi密码

        store_data("wifi", newdata);
    }

    // 设置地区 和风天气的地区id
    if (data[0] == 'l' && data[1] == 'o' && data[2] == 'c') {
        store_data("he_loc", newdata);
    }
}

void listen_uart(void *arg) {
    static const char *RX_TASK_TAG = "RX_TASK";
    char *data = (char *) malloc(RX_BUF_SIZE + 1);
    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM_0, data, RX_BUF_SIZE, 1000 / portTICK_PERIOD_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
            uart_event(data, rxBytes);
        }
    }
    free(data);
    vTaskDelete(NULL);
}


// 监听wifi的状态 并重新连接
static void event_handler(void *arg, esp_event_base_t event_base,
                          int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(CONN_TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(CONN_TAG, "connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        ESP_LOGI(CONN_TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(char *ssid, char *pwd) {
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
        ESP_EVENT_ANY_ID,
        &event_handler,
        NULL,
        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
        IP_EVENT_STA_GOT_IP,
        &event_handler,
        NULL,
        &instance_got_ip));


    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    size_t ssid_length = strlen(ssid);
    size_t pwd_length = strlen(pwd);
    memcpy(wifi_config.sta.ssid, ssid, ssid_length);
    memcpy(wifi_config.sta.password, pwd, pwd_length);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(CONN_TAG, "wifi_init_sta finished.");

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(CONN_TAG, "connected to ap SSID:%s password:%s", ssid, pwd);
        wifi_conned = 1;
    } else if (bits & WIFI_FAIL_BIT
    ) {
        ESP_LOGI(CONN_TAG, "Failed to connect to SSID:%s, password:%s", ssid, pwd);
    } else {
        ESP_LOGE(CONN_TAG, "UNEXPECTED EVENT");
    }
}

int get_wifi_conn() {
    return wifi_conned;
}

// MQTT
static void log_error_if_nonzero(const char *message, int error_code) {
    if (error_code != 0) {
        ESP_LOGE(CONN_TAG, "Last error %s: 0x%x", message, error_code);
    }
}

// MQTT事件处理函数
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(CONN_TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t) event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(CONN_TAG, "MQTT_EVENT_CONNECTED");
            msg_id = esp_mqtt_client_publish(client, "/topic/qos1", "data_3", 0, 1, 0);
            ESP_LOGI(CONN_TAG, "sent publish successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, "/topic/qos0", 0);
            ESP_LOGI(CONN_TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_subscribe(client, "/topic/qos1", 1);
            ESP_LOGI(CONN_TAG, "sent subscribe successful, msg_id=%d", msg_id);

            msg_id = esp_mqtt_client_unsubscribe(client, "/topic/qos1");
            ESP_LOGI(CONN_TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(CONN_TAG, "MQTT_EVENT_DISCONNECTED");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(CONN_TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            msg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
            ESP_LOGI(CONN_TAG, "sent publish successful, msg_id=%d", msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(CONN_TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(CONN_TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(CONN_TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(CONN_TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno",
                                     event->error_handle->esp_transport_sock_errno);
                ESP_LOGI(CONN_TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;
        default:
            ESP_LOGI(CONN_TAG, "Other event id:%d", event->event_id);
            break;
    }
}

#include "cJSON.h"

int tcp_client2(void) {
    char data[16];
    size_t length = sizeof(data);

    // 读取本地存储的地区代码
    err_t p = read_data("loc", data, &length);

    if (length == 0 || p == ESP_ERR_NVS_NOT_FOUND) {
        return 0;
    }


    tcp_client_t client;
    tcp_client_init(&client, "192.168.100.186", 10000);

    esp_err_t ret = tcp_client_connect(&client);
    if (ret != ESP_OK) {
        ESP_LOGE("TCP", "Failed to connect to server");
        vTaskDelete(NULL);
    }

    char request[24];
    memset(request, 0, sizeof(request));
    request[0] = 'n';
    request[1] = 'o';
    request[2] = 'w';
    for (int i = 3; i < length; ++i) {
        request[i] = data[i - 3];
    }
    ret = tcp_client_send(&client, request);
    if (ret != ESP_OK) {
        ESP_LOGE("TCP", "Failed to send request");
        vTaskDelete(NULL);
    }

    char rx_buffer[1024];
    memset(rx_buffer, 0, sizeof(rx_buffer));
    ret = tcp_client_receive(&client, rx_buffer, sizeof(rx_buffer));
    if (ret != ESP_OK) {
        ESP_LOGE("TCP", "Failed to receive response");
    }
    ESP_LOGI("TCP", "rece %s", rx_buffer);
    tcp_client_cleanup(&client);

    // 开始解析


    return 0;
}
