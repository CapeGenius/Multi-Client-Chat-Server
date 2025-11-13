#define _POSIX_C_SOURCE 200809L // allow newer POSIX functions like sigaction
#include <sys/socket.h>         // socket(), connect(), send(), recv()
#include <netinet/in.h>         // sockaddr_in struct
#include <arpa/inet.h>          // inet_addr, inet_pton helpers
#include <pthread.h>            // pthreads for multithreading
#include <stdio.h>              // printf, perror, etc
#include <stdlib.h>             // malloc, free, exit
#include <string.h>             // memset, memcpy
#include <unistd.h>             // close(), read(), write()
#include <time.h>               // timestamps (if needed)
#include <signal.h>             // signal handling (Ctrl+C)
#include <sys/select.h>         // select() for I/O multiplexing
#include <sys/time.h>           // timeval for select()
#include <errno.h>              // errno codes

#define MAX_MESSAGE_SIZE (1024 * 1024) // max size for reading terminal input (1MB)
volatile sig_atomic_t stop_client = 0; // flag flipped when Ctrl+C hits, safe for signal handlers

// Handles Ctrl+C by setting stop flag.
void handle_sigint(int sig){
    (void)sig; // we void cast to prevent compiler warnings
    stop_client = 1; // set the stop flag to 1
    printf("Caught Ctrl+C. Closing client connection.\n");
}


// Sets up the SIGINT (Ctrl+C) handler.
void install_sigint_handler() {
    struct sigaction sa;               // setup struct
    sa.sa_handler = handle_sigint;     // assign our handler
    sigemptyset(&sa.sa_mask);          // clear signal mask
    sa.sa_flags = 0;                   // no special flags
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction failed");    // print error if failed
        exit(EXIT_FAILURE);            // exit on failure
    }
}


// Sends user input to the server with a length header.
void* send_message(void* client_socket_ptr) {
    install_sigint_handler();   // install Ctrl+C handler
    int *sock_ptr = (int*)client_socket_ptr;  // cast pointer
    int client_socket = *sock_ptr;            // get socket fd
    char *message = NULL;                     // buffer for user input
    size_t len = 0;                           // getline() buffer length
    ssize_t read;                             // number of bytes read
    printf("Type your message (type 'exit' to quit, or press Ctrl+C): \n");
    while (!stop_client) {                    // loop until Ctrl+C or exit
        read = getline(&message, &len, stdin); // read input line
        if (read == -1) {
            // interrupted by signal (Ctrl+C)
            if (errno == EINTR) {
                printf("\ngetline interrupted by signal. Exiting input loop.\n");
                break;
            }
            // EOF or other error
            break;
        }
        if (read > 0 && message[read - 1] == '\n') {
            message[read - 1] = '\0';         // remove newline
            read--;
        }
        if (read == 0) continue;              // skip empty inputs
        if (strcmp(message, "exit") == 0) {   // user typed exit
            printf("Exit command received. Closing client connection.\n");
            break;
        }
        if (read > MAX_MESSAGE_SIZE) {        // enforce max size
            printf("Message too long. Limit is 1MB.\n");
            continue;
        }
        uint32_t msg_len = (uint32_t)read;    // actual message length
        uint32_t net_len = htonl(msg_len);    // convert to network order
        ssize_t sent_len = send(client_socket, &net_len, sizeof(net_len), 0);
        if (sent_len != sizeof(net_len)) {    // error sending length
            perror("Error sending message length");
            break;
        }
        ssize_t sent = send(client_socket, message, read, 0);
        if (sent < 0) {                       // error sending data
            perror("Error sending message");
            break;
        }
        printf("Message sent!\n");
    }
    free(message);                            // free buffer
    shutdown(client_socket, SHUT_RDWR);       // shutdown socket
    close(client_socket);                     // close it
    printf("Disconnected from server or user exited input.\n");
    return NULL;
}

// Reads messages from the server and prints them.
void* read_message(void* client_socket_ptr) {
    // extract the socket file descriptor
    int *sock_ptr = (int*)client_socket_ptr;
    int client_socket = *sock_ptr;
    // create a buffer pointer for recieved messages
    char *buffer = NULL;
    // loop forever until server disconnects or error occurs
    // we'll use this to track how many bytes were received
    ssize_t bytes_received;
    // this will store the 4-byte header (message length), net_len is 4 bytes 
    uint32_t net_len;
    // we use this to track how many bytes we've accumulated
    size_t total_received;
    printf("Read message started! \n");
    // loop to continuously receive messages
    while (stop_client != 1) {
        // STEP 1: Recieve the 4 byte length prefix
        // Loop while the TCP stream hasn't sent 4 bytes, cause we need these 4 bytes
        // to understand how long of a message we are going to need to buffer
        total_received = 0;
        while (total_received < sizeof(net_len)) {
            // read the amount of bytes recieved currently in our tcp_buffer (need to understand this line better)
            bytes_received = recv(client_socket, ((char*)&net_len) + total_received, 4 - total_received, 0);
                // if we are getting no bytes from our TCP stream, or a -1 value, something is wrong
                if (bytes_received <= 0) {
                    // server closed or error occured
                    goto disconnect;
                }
                // our new total amount of bytes we have gotten in total
                total_received += bytes_received;
            }
        // convert length to host byte order
        uint32_t msg_len = ntohl(net_len);
        // sanity check: reject absurdly large messages
        if (msg_len > MAX_MESSAGE_SIZE) {
            // make sure to print this in terminal (stderr)
            fprintf(stderr, "Message too large. Disconnecting.\n");
            goto disconnect;
        }
        // STEP 2: allocate the buffer for the incoming messages now that we know 
        // how long it should be
        buffer = malloc(msg_len + 1); // need extra byte for \0
        if (buffer == NULL) { // if malloc failed to give us memory
            perror("malloc failed");
            goto disconnect;
        }
        // STEP 3: recieve the full message body
        total_received = 0;
        while (total_received < msg_len) {
            // read the amount of message bytes recieved currently in our tcp_buffer in the kernal
            bytes_received = recv(client_socket, buffer + total_received, msg_len - total_received, 0);
            if (bytes_received <= 0) {
                // error or server disconnected
                free(buffer);
                goto disconnect;
            }
            total_received += bytes_received;
        }
        // add a null character to the end of the message
        buffer[msg_len] = '\0';
        // print out the servers message
        printf("\n[Server]: %s\n", buffer);
        // free the buffer
        free(buffer);
        buffer = NULL;
    }
    // disconnect logic
    disconnect:
        printf("\nDisconnected from server or error occured while reading.\n");
        return NULL;
}


// Connects to the server and returns the socket.
int connect_client(char* host, int* port){
    int client_socket = socket(AF_INET, SOCK_STREAM, 0); // create TCP socket
    if (client_socket < 0) {
        perror("Socket creation failed"); // error creating socket
    }
    printf("Socket created \n");
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));  // clear struct to all zeros
    server_addr.sin_family = AF_INET;              // IPv4
    server_addr.sin_port = htons(*port);           // convert port to network byte order
    // Convert string IP to binary form
    if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
        perror("Address does not exist");          // invalid IP string
        close(client_socket);                      // clean up socket
        return -1;
    }
    printf("IP Address reached. \n");
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");               // connect() error
        close(client_socket);                      // clean up socket
        return -1;
    }
    printf("Connection successful! \n");
    return client_socket;                           // return valid socket
}
