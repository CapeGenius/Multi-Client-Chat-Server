// router file that will be used to route to either a server / client executable
// from the client...it should take the client ip and port to route to
// from the server...it should take the server port 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    int choice;

    printf("Do you want to run a server (1) or client (2):\n");
    printf("1) Server\n");
    printf("2) Client\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    if (choice == 1) {
        execl("server/server", "server", NULL);
    } else if (choice == 2) {
        execl("client/client", "client", NULL);
    } else {
        printf("Invalid choice.\n");
    }

    return 0;
}
