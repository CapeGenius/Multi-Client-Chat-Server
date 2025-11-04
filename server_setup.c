#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <server_setup.h>
#define BUFFER_SIZE 1024

// listening socket function
int setup_listener(int port, int backlog) {
    int reuse = 1;
    int listener_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listener_socket < 0) {
        perror("Socket initialization failed");
        exit(1);
    }

    // ensures socket is reused
    setsockopt(listener_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    struct sockaddr_in servAddr = {0};
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    //binds to port
    if (bind(listener_socket, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        perror("Bind failed");
        exit(1);
    }

    //listens on socket
    if (listen(listener_socket, backlog) < 0) {
        perror("Listen failed");
        exit(1);
    }

    printf("Listening on port %d\n", port);
    return listener_socket;
}

// function for client handling for connected sockets --> will be used as a thread
void accept_connections(int listener_socket) {    

    while (1){
        int client_socket = accept(listener_socket, NULL, NULL);
        if (client_socket < 0) {
            perror("Error accepting client");
            exit(1);
        }

        int* client_socket_ptr = malloc(sizeof(int));
        *client_socket_ptr = client_socket;

        pthread_t thread;
        pthread_create(&thread, NULL, (void*)client_handling, (void*)client_socket_ptr);
        pthread_detach(thread);
    }    

}

void* client_handling(void* client_socket_ptr) {
    int clientSocket = *(int*)client_socket_ptr;
    free(client_socket_ptr);
    
    char buffer[BUFFER_SIZE]; 
    while(1) {
        int byte_count = read_info(clientSocket, buffer, BUFFER_SIZE);

        if (strcmp(buffer, "exit") == 0) {
            printf("Client requested exit.\n");
            shutdown(clientSocket, SHUT_RDWR); //uses client shutdown to shut down socket
            break;
        }

        send_info(clientSocket, buffer);
        
    }

    close(clientSocket);
}

// functions for sending and receiving info

int send_info(int socket, char* msg){
    char final_msg[BUFFER_SIZE + 20];  // fixed code to avoid messing with string literals
    snprintf(final_msg, sizeof(final_msg), "Server: %s\n", msg);

    int success = send(socket, final_msg, strlen(final_msg), 0);
    if (success == -1) {
        perror("Send failed");
    }

    return success;
    return NULL;
}

int read_info(int socket, char *buf, int len){
    int total = 0;
    while (total < len - 1) { //iterates through a string to receive all the bytes
        int c = recv(socket, buf + total, len - 1 - total, 0);
        if (c <= 0) {
            buf[total] = '\0';
            return c;
        }

        total += c;

        // Stop when we hit newline
        if (buf[total - 1] == '\n')
            break;
    }

    buf[total] = '\0'; // ending terminal of string
    // makes sure to replace the \n or \r with \0
    if (total > 0 && buf[total - 1] == '\n')
        buf[total - 1] = '\0';
    if (total > 1 && buf[total - 2] == '\r')
        buf[total - 2] = '\0';

    return total;
}