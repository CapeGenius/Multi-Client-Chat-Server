#include "logger.h"      // exposes the logger API and pulls in <time.h>
#include <stdlib.h>        // malloc/free declarations used by stdlib helpers
#include <string.h>        // memset/strcmp etc. (needed for snprintf)

static FILE *log_file = NULL;                            // shared log file handle
static pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER; // guards log_file access

int init_logger(const char *filename) {
  pthread_mutex_lock(&log_lock);    // only let one initializer open the file
  log_file = fopen(filename, "a"); // append keeps previous runs and creates file
  if (!log_file) {
    perror("Logger: failed to open file");
    pthread_mutex_unlock(&log_lock);
    return -1;
  }
  // Optional header if file is empty
  fseek(log_file, 0, SEEK_END); // jump to file end to test for emptiness
  if (ftell(log_file) == 0) {
    fprintf(log_file, "timestamp,client,message\n"); // CSV header row
    fflush(log_file);                                  // make sure header hits disk
  }
  pthread_mutex_unlock(&log_lock); // unlock after initialization completes
  return 0;
}

void log_message(const char *client, const char *message) {
  if (!log_file)
    return; // logging before init is a no-op

  pthread_mutex_lock(&log_lock); // serialize writes across threads

  // converts epoch time to localtime
  time_t now = time(NULL);                 // capture current wall clock time
  struct tm *t = localtime(&now);          // convert to local calendar time /servers time zone
  char timebuf[64];                        // formatted timestamp buffer
  strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", t); // strftime formats struct tm into the provided pattern

  // Escape double quotes for CSV safety
  char msgbuf[1024];
  snprintf(msgbuf, sizeof(msgbuf), "\"%s\"", message); // wrap message in quotes to protect commas

  fprintf(log_file, "%s,%s,%s\n", timebuf, client, msgbuf); // timestamped CSV row
  fflush(log_file);                                           // immediate durability for debugging, observing real-time behaviour 

  pthread_mutex_unlock(&log_lock); // release so other threads can log
}

void close_logger() {
  pthread_mutex_lock(&log_lock); // coordinate shutdown with active writers
  if (log_file) {
    fclose(log_file);           // flush buffers and close descriptor
    log_file = NULL;            // mark logger as inactive for future calls
  }
  pthread_mutex_unlock(&log_lock);
}
