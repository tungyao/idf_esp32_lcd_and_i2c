#include <stdio.h>
#include "conn.h"

#include <esp_timer.h>
#include <strings.h>
#include <driver/gpio.h>
#include <driver/uart.h>
#include <freertos/projdefs.h>
#include <lwip/sockets.h>

#include "sto.h"

static void IRAM_ATTR key_isr_handler(void *arg) {
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(key_queue, &gpio_num, NULL);
}

void conn_keys_init() {
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_ANYEDGE, // Disable interrupt
        .mode = GPIO_MODE_INPUT, // Set as Input
        .pin_bit_mask = (1ULL << IO19), // Bitmask
        .pull_down_en = 1, // Enable pull-up
        .pull_up_en = 0, // Enable pull-up
    };
    gpio_config(&io_conf);

    gpio_config_t io_conf2 = {
        .intr_type = GPIO_INTR_ANYEDGE, // Disable interrupt
        .mode = GPIO_MODE_INPUT, // Set as Input
        .pin_bit_mask = (1ULL << IO18), // Bitmask
        .pull_up_en = GPIO_PULLUP_ENABLE, // Enable pull-up
    };
    gpio_config(&io_conf2);

    key_queue = xQueueCreate(1, sizeof(uint32_t));
    gpio_intr_enable(IO19);
    gpio_intr_enable(IO18);

    gpio_install_isr_service(0);
    // gpio_isr_register(IO19, key_isr_handler, NULL, 0, 0);
    gpio_isr_handler_add(IO19, key_isr_handler, (void *) IO19);
    // gpio_isr_handler_add(IO18, key_isr_handler, (void *) IO18);
}

void listen_config_key() {
    uint32_t gpio_num;
    ESP_LOGI("RX_TASK_TAG", "uart_driver_install");
    // Set UART receive callback function
    while (1) {
        //
        if (xQueueReceive(key_queue, &gpio_num, portMAX_DELAY)) {
            if (gpio_num == IO19  && input_mode == 0) {
                ESP_LOGI("RX_TASK_TAG", "into uart get wifi password");

                int64_t press_start_time = esp_timer_get_time() / 1000 / 1000; // 获取当前时间
                ESP_LOGI("RX_TASK_TAG", "%lld", press_start_time);
                if (gpio_get_level(IO19) == 1) {
                    vTaskDelay(pdMS_TO_TICKS(5000));
                    if (gpio_get_level(IO19) == 1) {
                        input_mode = 1;
                        ESP_LOGI("RX_TASK_TAG", "xxxxxxxxxxxxxxxxxxxxx");
                    }
                }
            }
        }
    }
}

void listen_uart() {
    while (1 && input_mode == 1) {
        vTaskDelay(pdMS_TO_TICKS(1000)); // 延时，降低CPU占用
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

    wifi_config_t wifi_config;
    wifi_sta_config_t sta;
    *sta.ssid = (uint8_t) *ssid;
    *sta.password = (uint8_t) *pwd;
    wifi_config.sta = sta;

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
    if
    (bits & WIFI_CONNECTED_BIT
    ) {
        ESP_LOGI(CONN_TAG, "connected to ap SSID:%s password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else if
    (bits & WIFI_FAIL_BIT
    ) {
        ESP_LOGI(CONN_TAG, "Failed to connect to SSID:%s, password:%s",
                 EXAMPLE_ESP_WIFI_SSID, EXAMPLE_ESP_WIFI_PASS);
    } else {
        ESP_LOGE(CONN_TAG, "UNEXPECTED EVENT");
    }
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


void tcp_client(void) {
    char rx_buffer[256];
    char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;
    char *payload = "weather:chongqing";
    struct sockaddr_in dest_addr;
    inet_pton(AF_INET, host_ip, &dest_addr.sin_addr);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = PORT;
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;

    int sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (sock < 0) {
        ESP_LOGE(CONN_TAG, "Unable to create socket: errno %d", errno);
        return;
    }
    ESP_LOGI(CONN_TAG, "Socket created, connecting to %s:%d", host_ip, PORT);

    int err = connect(sock, (struct sockaddr *) &dest_addr, sizeof(dest_addr));
    if (err != 0) {
        ESP_LOGE(CONN_TAG, "Socket unable to connect: errno %d", errno);
        return;
    }
    ESP_LOGI(CONN_TAG, "Successfully connected");

    while (1) {
        int err = send(sock, payload, strlen(payload), 0);
        if (err < 0) {
            ESP_LOGE(CONN_TAG, "Error occurred during sending: errno %d", errno);
            break;
        }

        int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        // Error occurred during receiving
        if (len < 0) {
            ESP_LOGE(CONN_TAG, "recv failed: errno %d", errno);
            break;
        }
        // Data received
        else {
            rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
            ESP_LOGI(CONN_TAG, "Received %d bytes from %s:", len, host_ip);
            ESP_LOGI(CONN_TAG, "%s", rx_buffer);
            break;
        }
    }

    ESP_LOGE(CONN_TAG, "Shutting down socket and restarting...");
    shutdown(sock, 0);
    close(sock);
}
