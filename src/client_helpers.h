#pragma once
#ifndef CLIENT_HELPERS_H
#define CLIENT_HELPERS_H

#include <netinet/in.h>   
#include <pthread.h>

#define MAX_MESSAGE_SIZE (1024 * 1024)

void* send_message(void* client_socket_ptr);
void* read_message(void* client_socket_ptr);
int connect_client(char* host, int* port);
void handle_sigint(int sig);
void prompt_ip(char* buffer, size_t size);
int prompt_port();

#endif // CLIENT_HELPERS_H