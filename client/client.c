#include "client_helpers.h"
#include <pthread.h>
#include <signal.h>
#include <stdio.h>

int main() {
  signal(SIGINT, handle_sigint); // basic Ctrl+C handler
  char ip[16];                   // buffer for IP address
  prompt_ip(ip, sizeof(ip));     // ask user for IP
  int port = prompt_port();      // ask user for port

  int client_socket = connect_client(ip, &port); // connect to server

  pthread_t receive_thread;
  pthread_t send_thread;

  pthread_create(&receive_thread, NULL, (void *)read_message,
                 (void *)&client_socket); // thread to read
  pthread_create(&send_thread, NULL, (void *)send_message,
                 (void *)&client_socket); // thread to send
  pthread_join(receive_thread, NULL);     // wait for read thread
  pthread_join(send_thread, NULL);        // wait for send thread

  return 0;
}

void prompt_ip(char *buffer, size_t size) {
  puts("Type Remote IP Address:"); // ask user for IP
  scanf("%15s", buffer);           // read IP string
}

int prompt_port() {
  int port;
  puts("Type Port:"); // ask user for port
  scanf("%d", &port); // read port number

  return port;
}
