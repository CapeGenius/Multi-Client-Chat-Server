#pragma once
#ifndef SERVER_SETUP_H
#define SERVER_SETUP_H

#include <netinet/in.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

// --- Function Declarations ---

// creates TCP listener socket
int setup_listener(int port, int backlog);

// Accepts client connections and creates cliend handling threads
void accept_connections(int listener_socket);

// handles every single server-client connection
void* client_handling(void* client_socket_ptr);

// Helper functions for reading and writing
int send_info(int socket, char* msg);
int read_info(int socket, char *buf, int len);

#endif // SERVER_UTILS_H
