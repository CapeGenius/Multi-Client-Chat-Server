#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "server_setup.h"
#include "logger.h"
#define BUFFER_SIZE 1024

// declare a poll or buffer that holds all message being sent --> send back message through every single socket

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
        if (byte_count > 0) {
            log_message("client", buffer);
        }
        if (strcmp(buffer, "exit") == 0) {
            printf("Client requested exit.\n");
            shutdown(clientSocket, SHUT_RDWR); //uses client shutdown to shut down socket
            break;
        }

        send_info(clientSocket, buffer);
        
    }

    close(clientSocket);
    return NULL;
}

// reimplemented send_info to follow client side 4byte prefix profile
int send_info(int socket, char* msg){
    char final_msg[BUFFER_SIZE + 20]; 
    int final_msg_len = snprintf(final_msg, sizeof(final_msg), "Server: %s", msg); 

    //ensures that the message is not empty / doesn't exist final message size
    if (final_msg_len < 0 || final_msg_len >= sizeof(final_msg)) {
        fprintf(stderr, "Error or truncation in snprintf.\n");
        return -1;
    }

    // create 4-byte length prefix for message
    uint32_t msg_len = (uint32_t)final_msg_len;
    uint32_t net_len = htonl(msg_len);

    // send 4 byte prefix
    int sent_len = send(socket, &net_len, sizeof(net_len), 0);
    if (sent_len != sizeof(net_len)) {
        perror("Error sending message length from server");
        return -1;
    }

    // Send message after sending prefix
    int sent_msg = send(socket, final_msg, final_msg_len, 0);
    if (sent_msg != final_msg_len) {
        perror("Send message body failed from server");
        return -1;
    }

    return sent_len + sent_msg; // Return total bytes sent
}

// reimplemented read_info to follow client side 4byte prefix profile
int read_info(int socket, char *buf, int len){
    uint32_t net_len;
    uint32_t msg_len; // 
    ssize_t bytes_received;
    size_t total_received;
    
    //read 4 byte prefix 
    total_received = 0;
    while (total_received < sizeof(net_len)) {
        // Use sizeof(net_len) - total_received to ensure we only read the remaining bytes
        bytes_received = recv(socket, ((char*)&net_len) + total_received, 
                              sizeof(net_len) - total_received, 0);

        if (bytes_received <= 0) {
            // Client closed connection or error
            if (total_received > 0) {
                 fprintf(stderr, "Partial length prefix received. Disconnecting.\n");
            }
            return bytes_received; // 0 for clean close, -1 for error
        }
        total_received += bytes_received;
    }
    
    // Convert length to host byte order
    msg_len = ntohl(net_len);

    // ensure message fits in the provided buffer (len is max buffer size)
    if (msg_len >= (uint32_t)len) {
        fprintf(stderr, "Server buffer too small for incoming message. Disconnecting.\n"); 
        // but for now, we'll just return an error.
        return -2; // Custom error for buffer too small
    }
    
    // if first check is passed, read the message body
    total_received = 0;
    while (total_received < msg_len) {
        // Read the remaining bytes of the message body
        bytes_received = recv(socket, buf + total_received, msg_len - total_received, 0);
        
        if (bytes_received <= 0) {
            // Client disconnected while sending body or error
            fprintf(stderr, "Client disconnected during message body transfer.\n");
            return bytes_received;
        }
        total_received += bytes_received;
    }

    // Null-terminate the received message
    buf[msg_len] = '\0'; 

    return (int)msg_len;
}