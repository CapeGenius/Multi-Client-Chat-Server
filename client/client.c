#include "client_helpers.h"
#include <stdio.h>

int main() {
    char ip[16];
    prompt_ip(ip, sizeof(ip));
    int port = prompt_port();

    int client_socket = connect_client(ip, &port);

    pthread_t receive_thread;
    pthread_t send_thread; 

    pthread_create(&receive_thread, NULL, (void*)read_message, (void*) &client_socket);
    pthread_create(&send_thread, NULL, (void*)send_message, (void*) &client_socket);
    pthread_join(receive_thread, NULL);
    pthread_join(send_thread, NULL);


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
