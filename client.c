#include <sys/socket.h>   
#include <netinet/in.h>   
#include <arpa/inet.h>    
#include <pthread.h>      
#include <stdio.h>        
#include <stdlib.h>       
#include <string.h>       
#include <unistd.h>
#include <time.h> 

#define MAX_MESSAGE_SIZE (1024 * 1024) // define a max message size for our terminal input 1MB

// function to send messages, client_socket_ptr is defined in main_client
void* send_message(void* client_socket_ptr) {
    // gives us the file descriptor of the client socket we want to use in order to communicate
    int *sock_ptr = (int*)client_socket_ptr;
    int client_socket = *sock_ptr;
    // initializing our message buffer to store the stdin input
    char *message = NULL;
    // introduce to more variables that getline() needs
    size_t len = 0; // tracks the allocated buffer size for getline()
    ssize_t read; // will store the number of characters read (-1 EOF)
    // prompt the user for a message
    printf("Type your message (Ctrl+D to quit): \n"); 
    // while there is a non EOF (end of file) message typed into terminal
    while ((read = getline(&message, &len, stdin)) != -1) {
        // remove trailing newline
        if (read > 0 && message[read - 1] == '\n') {
            message[read - 1] = '\0';
            read--;
        }
        // if our message is empty, wait for the next message or ctrl d from the user
        if (read == 0) {
            continue;
        }
        // check the max message size
        if (read > MAX_MESSAGE_SIZE) {
            printf("Message too long. Limit is 1MB.\n");
            continue;
        }
        // send 4-byte length prefix to the server
        uint32_t msg_len = (uint32_t)read;
        uint32_t net_len = htonl(msg_len);
        // send function to do that
        ssize_t sent_len = send(client_socket, &net_len, sizeof(net_len), 0);
        // ensure that the amount of bytes sent to the server is as much as we determines to send
        if (sent_len != sizeof(net_len)) {
            perror("Error sending message length");
            break;
        }
        // send the message to the server
        ssize_t sent = send(client_socket, message, read, 0);
        if (sent < 0) {
            perror("Error sending message");
            break;
        }
        // Print out that the message sent successfully
        printf("Message sent!\n");
    }
    // message is dynamically allocated using malloc underneath the hood of getline, so we must free it
    free(message);
    // we close the client socket if our user pressed Ctrl+D
    close(client_socket);
    // Either the user has ended the client communication with the server
    // or send() failed
    printf("Disconnected from server or user exited input.\n");
    // this function has a void* return type, and NULL is a void* under the hood
    return NULL;

    // dont worry about thread and sockets...
    // we create another main_client (this will handle threading)
}
// function to read messages from the server
void* read_message(void* client_socket_ptr) {
    // extract the socket file descriptor
    int *sock_ptr = (int*)client_socket_ptr;
    int client_socket = *sock_ptr;
    // create a buffer pointer for recieved messages
    char *buffer = NULL;
    // loop forever until server disconnects or error occurs
    while (1) {
        // we'll use this to track how many bytes were received
        ssize_t bytes_received = 0;
        // this will store the 4-byte header (message length)
        uint32_t net_len = 0;
        // we use this to track how many bytes we've accumulated
        size_t total_received = 0;
        // loop to continuously receive messages
    
}


}