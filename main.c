#include "server_setup.h"
#include <stdio.h>

int main() {
    int listener_socket = setup_listener(8080, 10);
    accept_connections(listener_socket);
    return 0;
}
