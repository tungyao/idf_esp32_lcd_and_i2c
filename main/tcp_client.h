//
// Created by yao19 on 2024/7/17.
//

#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H
#include <esp_err.h>

typedef struct {
    char *host;
    int port;
    int socket;
} tcp_client_t;

void tcp_client_init(tcp_client_t *client, const char *host, int port);
void tcp_client_cleanup(tcp_client_t *client);
esp_err_t tcp_client_connect(tcp_client_t *client);
esp_err_t tcp_client_send(tcp_client_t *client, const char *data);
int tcp_client_receive(tcp_client_t *client, char *buffer, int buffer_size);



#endif //TCP_CLIENT_H
