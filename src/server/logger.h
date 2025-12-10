#ifndef LOGGER_H
#define LOGGER_H

#include <pthread.h>
#include <stdio.h>
#include <time.h>

// Initialize logger with file path
int init_logger(const char *filename);

// Log a message (thread-safe)
void log_message(const char *client, const char *message);

// Close logger
void close_logger();

#endif
