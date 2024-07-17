#include "tcp_client.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_tls.h"
#include "esp_netif.h"
#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#define TAG "TCP_CLIENT"

void tcp_client_init(tcp_client_t *client, const char *host, int port) {
    client->host = (char *)host;
    client->port = port;
    client->socket = -1;
}

void tcp_client_cleanup(tcp_client_t *client) {
    if (client->socket != -1) {
        close(client->socket);
        client->socket = -1;
    }
}

esp_err_t tcp_client_connect(tcp_client_t *client) {
    struct sockaddr_in server_addr;

    client->socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client->socket < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        return ESP_FAIL;
    }

    struct hostent *host_ip = gethostbyname(client->host);
    if (host_ip == NULL) {
        ESP_LOGE(TAG, "Unable to get IP address of host %s", client->host);
        return ESP_FAIL;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(client->port);
    server_addr.sin_addr.s_addr = *((uint32_t *)host_ip->h_addr);

    int err = connect(client->socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
        close(client->socket);
        client->socket = -1;
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Successfully connected to %s:%d", client->host, client->port);
    return ESP_OK;
}

esp_err_t tcp_client_send(tcp_client_t *client, const char *data) {
    int err = send(client->socket, data, strlen(data), 0);
    if (err < 0) {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "Data sent: %s", data);
    return ESP_OK;
}

esp_err_t tcp_client_receive(tcp_client_t *client, char *buffer, int buffer_size) {
    int len = recv(client->socket, buffer, buffer_size - 1, 0);
    if (len < 0) {
        ESP_LOGE(TAG, "Receive failed: errno %d", errno);
        return ESP_FAIL;
    } else if (len == 0) {
        ESP_LOGW(TAG, "Connection closed");
        return ESP_FAIL;
    } else {
        buffer[len] = '\0'; // Null-terminate the buffer
        ESP_LOGI(TAG, "Received %d bytes: %s", len, buffer);
        return ESP_OK;
    }
}
