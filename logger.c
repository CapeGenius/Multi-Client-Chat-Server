#include "logger.h"
#include <stdlib.h>
#include <string.h>

static FILE *log_file = NULL;
static pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER;

int init_logger(const char *filename) {
    pthread_mutex_lock(&log_lock);
    log_file = fopen(filename, "a");
    if (!log_file) {
        perror("Logger: failed to open file");
        pthread_mutex_unlock(&log_lock);
        return -1;
    }
    // Optional header if file is empty
    fseek(log_file, 0, SEEK_END);
    if (ftell(log_file) == 0) {
        fprintf(log_file, "timestamp,client,message\n");
        fflush(log_file);
    }
    pthread_mutex_unlock(&log_lock);
    return 0;
}

void log_message(const char *client, const char *message) {
    if (!log_file) return;

    pthread_mutex_lock(&log_lock);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timebuf[64];
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", t);

    // Escape double quotes for CSV safety
    char msgbuf[1024];
    snprintf(msgbuf, sizeof(msgbuf), "\"%s\"", message);

    fprintf(log_file, "%s,%s,%s\n", timebuf, client, msgbuf);
    fflush(log_file);

    pthread_mutex_unlock(&log_lock);
}

void close_logger() {
    pthread_mutex_lock(&log_lock);
    if (log_file) {
        fclose(log_file);
        log_file = NULL;
    }
    pthread_mutex_unlock(&log_lock);
}
