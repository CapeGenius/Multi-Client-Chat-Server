#pragma once
#ifndef SERVER_SETUP_H
#define SERVER_SETUP_H

#include <netinet/in.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

// --- Function Declarations ---

// creates TCP listener socket
int setup_listener(int port, int backlog);

// Accepts client connections and creates client handling threads
void accept_connections(int listener_socket);

// Thread handlers for client connections
void* read_handling(void* client_socket_ptr);
void* write_handling();

// Mutex helpers for client count
void increment_client_count(void);
void decrement_client_count(void);

// Message helpers
void reset_message(char* buffer);
void write_message(int client_socket);
void reset_flag();

// Helper functions for reading and writing
int send_info(int socket, char* msg);
int read_info(int socket, char *buf, int len);

#endif // SERVER_SETUP_H
