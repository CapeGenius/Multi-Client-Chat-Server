// router file that will be used to route to either a server / client executable
// from the client...it should take the client ip and port to route to
// from the server...it should take the server port 

#include <stdio.h>
#include <stdlib.h>

int main() {
    int choice;
    char ip[100];
    int port;

    printf("Run as:\n");
    printf("1) Server\n");
    printf("2) Client\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    printf("Enter port: ");
    scanf("%d", &port);

    if (choice == 1) {
        char command[100];
        sprintf(command, "./server %d", port);
        system(command);
    } else if (choice == 2) {
        printf("Enter server IP: ");
        scanf("%s", ip);

        char command[200];
        sprintf(command, "./client %s %d", ip, port);
        system(command);
    } else {
        printf("Invalid choice.\n");
    }

    return 0;
}
