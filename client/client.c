#include "client_helpers.h"
#include <stdio.h>

int main() {
    char ip[16];
    prompt_ip(ip, sizeof(ip));
    int port = prompt_port();

    int client_socket = connect_client(ip, &port);

    close(client_socket);

    return 0;
}


void prompt_ip(char* buffer, size_t size) {
    puts("Type Remote IP Address:");
    scanf("%15s", buffer);
}

int prompt_port() {
    int port;
    puts("Type Port:");
    scanf("%d", &port);

    return port;
}
