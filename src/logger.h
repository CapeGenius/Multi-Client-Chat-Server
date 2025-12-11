#ifndef LOGGER_H
#define LOGGER_H

// Initialize logger with file path
int init_logger(const char *filename);

// Log a message (thread-safe)
void log_message(const char *message, const char *client);

// Close logger
void close_logger();

#endif
