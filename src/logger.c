#include <pthread.h>
#include <time.h>
#include <stdio.h>

#include "logger.h"
#define TIMEBUF_SIZE 64
#define MSGBUF_SIZE 1024
static FILE *log_file = NULL; // file on disk we read/
static pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER; // create our mutex and initialize its state

int init_logger(const char *filename) {
  pthread_mutex_lock(&log_lock);
  log_file = fopen(filename, "a");
  if (!log_file) {
    perror("Logger: failed to open file");
    pthread_mutex_unlock(&log_lock);
    return -1;
  }
  // Optional header if file is empty
  fseek(log_file, 0, SEEK_END); // Move the cursor to offset 0 relative to the end of the file
  if (ftell(log_file) == 0) { // ftell returns the current cursor position
    fprintf(log_file, "timestamp,client,message\n"); // writes the csv header into a internal memory buffer
    fflush(log_file); // flushes to disk immediately
  }
  pthread_mutex_unlock(&log_lock);
  return 0;
}

void log_message(const char *message, const char *client) {
  if (!log_file){
    return; 
  }
  pthread_mutex_lock(&log_lock);
  time_t now = time(NULL);
  struct tm *time_info = localtime(&now); // localtime takes the timestamp stored at now, converts it to local time, and fills a static struct tm with the converted values. t points to that struct.
  char timebuf[TIMEBUF_SIZE];
  strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", time_info); // fills timebuf with a formatted, human-readable timestamp.

  // Escape double quotes for CSV safety
  char msgbuf[MSGBUF_SIZE]; 
  snprintf(msgbuf, sizeof(msgbuf), "\"%s\"", message); // snprintf is a safe version of sprintf. It writes into msgbuf without overflowing.

  fprintf(log_file, "%s,%s,%s\n", timebuf, client, msgbuf);
  fflush(log_file);

  pthread_mutex_unlock(&log_lock);
}

void close_logger() {
  pthread_mutex_lock(&log_lock);
  if (log_file) {
    if (fclose(log_file)!= 0){
      perror("Logger: failed to close file");
    }
    log_file = NULL;
  }
  pthread_mutex_unlock(&log_lock);
}
