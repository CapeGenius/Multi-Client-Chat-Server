#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main() {

    int servSockD = socket(AF_INET, SOCK_STREAM, 0);

    // checking if the file descriptor is created
    if (servSockD < 0){
        perror("Socket initialization failed \n");
        exit(1);
    }

    printf("Socket created");

    // declaring a struct for the IP address
    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(9001);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    // ensuring that the socket binds to port
    if (bind(servSockD, (struct sockaddr_in*)&servAddr, sizeof(servAddr)) < 0) {
        perror("Failed to bind to port");
        exit(1);
    }

    printf("Socket bound to port");

    //ensure that socket listens to new connections
    if (listen(servSockD, 1) < 0) {
        perror("Couldn't listen on socket");
    }

    printf("Socket listening on port");

    int clientSocket = accept(servSockD, NULL, NULL);
    if (clientSocket < 0) {
        perror("Client Connection Faileed");
        exit(1);
    }

    printf("Client connected to server");

    //create a buffer from client
    char buffer[1024] = {0};
    recv(clientSocket, buffer, sizeof(buffer), 0);

    printf("Message from client: %s\n", buffer);

    close(clientSocket);
    close(servSockD);

    return 0;

    

}