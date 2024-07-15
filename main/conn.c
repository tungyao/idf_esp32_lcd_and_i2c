#include <stdio.h>
#include "conn.h"

#include <esp_timer.h>
#include <strings.h>
#include <driver/gpio.h>
#include <driver/uart.h>
#include <freertos/projdefs.h>

#include "sto.h"

static void IRAM_ATTR key_isr_handler(void *arg) {
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(key_queue, &gpio_num, NULL);
}

void conn_keys_init() {
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_POSEDGE, // Disable interrupt
        .mode = GPIO_MODE_INPUT, // Set as Input
        .pin_bit_mask = (1ULL << IO19), // Bitmask
        .pull_up_en = GPIO_PULLUP_DISABLE, // Enable pull-up
    };
    gpio_config(&io_conf);

    gpio_config_t io_conf2 = {
        .intr_type = GPIO_INTR_POSEDGE, // Disable interrupt
        .mode = GPIO_MODE_INPUT, // Set as Input
        .pin_bit_mask = (1ULL << IO18), // Bitmask
        .pull_up_en = GPIO_PULLUP_DISABLE, // Enable pull-up
    };
    gpio_config(&io_conf2);

    key_queue = xQueueCreate(10, sizeof(uint32_t));
    gpio_install_isr_service(0);
    gpio_isr_handler_add(IO19, key_isr_handler, (void *) IO19);
    gpio_isr_handler_add(IO18, key_isr_handler, (void *) IO18);
}

void listen_config_key() {
    uint32_t gpio_num;
    while (pdTRUE) {
        //
        if (xQueueReceive(key_queue, &gpio_num, portMAX_DELAY)) {
            if (gpio_num == IO19) {
                int64_t press_start_time = esp_timer_get_time(); // 获取当前时间
                while (1) {
                    if (gpio_get_level(IO19) == 1) {
                        // 检测按键是否被按下
                        vTaskDelay(pdMS_TO_TICKS(100)); // 短暂延时，消抖
                        if (gpio_get_level(IO19) == 1) {
                            // 再次检测，确认按键状态
                            int64_t current_time = esp_timer_get_time();
                            if ((current_time - press_start_time) >= LONG_PRESS_TIME) {
                                // 进入uart获取wifi相关的数据并写入
                                char *data = (char *) malloc(RD_BUF_SIZE + 1);
                                const int rxBytes = uart_read_bytes(UART_NUM_0, data, RD_BUF_SIZE,
                                                                    1000 / portTICK_PERIOD_MS);
                                if (rxBytes > 0) {
                                    data[rxBytes] = 0;
                                    ESP_LOGI("RX_TASK_TAG", "Read %d bytes: '%s'", rxBytes, data);
                                    ESP_LOG_BUFFER_HEXDUMP("RX_TASK_TAG", data, rxBytes, ESP_LOG_INFO);

                                    store_data("wifi", data);
                                }
                            }
                        }
                    }
                    vTaskDelay(pdMS_TO_TICKS(20)); // 延时，降低CPU占用
                }
            } else {
            }
        }
    }
}


// 连接WiFi的函数
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

void wifi_init_sta(void) {
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
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(CONN_TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(CONN_TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(CONN_TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else {
        ESP_LOGE(CONN_TAG, "UNEXPECTED EVENT");
    }
}


// MQTT
static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(CONN_TAG, "Last error %s: 0x%x", message, error_code);
    }
}
// MQTT事件处理函数
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(CONN_TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32 "", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id) {
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
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(CONN_TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));

        }
        break;
    default:
        ESP_LOGI(CONN_TAG, "Other event id:%d", event->event_id);
        break;
    }
}


// 初始化MQTT客户端的方法
void mqtt_app_start() {
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.port = 10000,
        .broker.address.uri = "mqtt://"

    };
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}
