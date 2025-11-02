#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#define BUFFER_SIZE 1024

int main() {
    int reuse = 1;
    int listener_d = socket(AF_INET, SOCK_STREAM, 0);
    int connection_size = 10;

    // checking if the file descriptor is created
    if (listener_d < 0){
        perror("Socket initialization failed \n");
        exit(1);
    }

    printf("Socket created \n");

    if (setsockopt(listener_d, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(int)) == -1)
    {
        perror("Can't set the reuse option on the socket\n");
    }

    // declaring a struct for the IP address
    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(9001);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    // ensuring that the socket binds to port
    if (bind(listener_d, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        perror("Failed to bind to port \n");
        exit(1);
    }

    printf("Socket bound to port \n");

    //ensure that socket listens to new connections
    if (listen(listener_d, connection_size) < 0) {
        perror("Couldn't listen on socket \n");
    }

    printf("Socket listening on port \n");

    while (1)
    {
        int clientSocket = accept(listener_d, NULL, NULL);

        if (clientSocket < 0) {
            perror("Client Connection Faileed \n");
            exit(1);
        }

        pid_t child_process = fork();

        if (child_process == 0) {
            close(listener_d);

            client_handling(clientSocket);
            exit(0);
        } 
        else
            close(clientSocket);
    }

    close(listener_d);

    return 0;

}

int send_info(int socket, char* msg){
    // dereferences client socket and sends message to socket
    char new_line[] = "\n";
    char server_msg[] = "Server:";

    char* temp_msg = strcat(msg, new_line);
    char* final_msg = strcat(server_msg, temp_msg);

    int success = send(socket, final_msg, strlen(msg), 0);
    if (success == -1) {
        perror("Send failed");
    }

    return success;
}

int read_info(int socket, char *buf, int len){
    char *s = buf;
    int slen = len;
    int c = recv(socket, s, slen, 0);

    while ((c > 0) && (s[c-1] != '\n')) {
        s += c; slen -= c;
        c = recv(socket, s, slen, 0);
    }
    if (c < 0)
        return c;
    else if (c == 0) {
        buf[0] = '\0';
        return 0;
    }
    else
        s[c -1] = '\0';
    return len - slen;
}

void client_handling(int clientSocket) {
    char buffer[BUFFER_SIZE]; 
    while(1) {
        int byte_count = read_info(clientSocket, buffer, BUFFER_SIZE);

        if (strcmp(buffer, "exit") == 0) {
            printf("Client requested exit.\n");
            break;
        }

        send_info(clientSocket, buffer);
        
    }

    close(clientSocket);
}