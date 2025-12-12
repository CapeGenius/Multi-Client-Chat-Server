#include "client_helpers.h"
#include <signal.h> 
#include <pthread.h>
#include <stdio.h>

#define IP_BYTE 16

// I wrote this function, with help to scope it from chatGPT. Chat helped me figure out what was impotant for how 
// to scope it. I also asked a friend while I was calling him how to approach it and we conversed about it verbally.
int main() {
    signal(SIGINT, handle_sigint); // this is for handling OS signals (like when we get sigint from cntr+c)
    char ip_address[IP_BYTE]; // 16 bytes to store ip address
    prompt_ip(ip_address, sizeof(ip_address)); // puts our ip address in our ip buffer
    int port = prompt_port(); // 
    int client_socket = connect_client(ip_address, &port); // connect the client socket
    pthread_t receive_thread; // create a place to store my first thread descriptor
    pthread_t send_thread; // create a place to store my second thread descriptor
    pthread_create(&receive_thread, NULL, (void*)read_message, (void*) &client_socket); // runs read message, and uses client_socket for communication 
    pthread_create(&send_thread, NULL, (void*)send_message, (void*) &client_socket); // runs send message, and uses client_socket for communication
    pthread_join(receive_thread, NULL); // wait for recieve thread to finish
    pthread_join(send_thread, NULL); // wait for send thread to finish
    return 0;
}

// I wrote this function
void prompt_ip(char* buffer, size_t size) {
    puts("Type Remote IP Address:"); // simple way of doing a printf...also goes to next line
    scanf("%15s", buffer); // looks for user input and puts in buffer
}

// I wrote this function
int prompt_port() {
    int port;
    puts("Type Port:"); // puts to stdout
    scanf("%d", &port); // takes in what we types into to the port.
    return port; 
}
