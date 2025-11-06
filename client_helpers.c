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

/**
Function to send messages to the server. This function...
    -Reads user input from stdin using getline(), dynamically resizing as needed.
    -Strips the trailing newline character from the message.
    -Prefixes the message with a 4-byte length header in network byte order.
    -Sends both the length header and the actual message over the TCP socket.
 */
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
}

/**
Function to read messages from the server. This function...
    -Reads from the kernel’s TCP buffer into our own dynamically allocated buffer.
    -Null-terminates it so printf() can treat it as a string.
    -Prints the received message to stdout
 */
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
    // loop to continuously receive messages
    while (1) {
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
        close(client_socket);
        return NULL;
}

int connect_client(char* host, int* port){
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket < 0) {
        perror("Socket creation failed");
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));  // important!
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // Convert string IP to binary form
    if (inet_pton(AF_INET, host, &server_addr.sin_addr) <= 0) {
        perror("Address does not exist");
        close(client_socket);
        return -1;
    }

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(client_socket);
        return -1;
    }

    printf("Connected to %s:%d\n", host, port);

    return client_socket;
}