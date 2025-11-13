#pragma once
#ifndef CLIENT_HELPERS_H
#define CLIENT_HELPERS_H

#include <netinet/in.h>   
#include <pthread.h>

#define MAX_MESSAGE_SIZE (1024 * 1024)

/**
 * Sends user input to the server with a length header.
 */
void* send_message(void* client_socket_ptr);

/**
 * Reads messages from the server and prints them.
 */
void* read_message(void* client_socket_ptr);

/**
 * Connects to the server using the given host and port.
 * Returns the socket file descriptor or -1 on failure.
 */
int connect_client(char* host, int* port);

/**
 * Handles Ctrl+C by setting the stop flag.
 */
void handle_sigint(int sig);

/**
 * Prompts the user for an IP address and stores it in buffer.
 */
void prompt_ip(char* buffer, size_t size);

/**
 * Prompts the user for a port number and returns it.
 */
int prompt_port();


#endif // CLIENT_HELPERS_H