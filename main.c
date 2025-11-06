#include "server_setup.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    if (init_logger("server_log.csv") != 0) {
        fprintf(stderr, "Failed to initialize logger\n");
        exit(1);
    }

    int listener_socket = setup_listener(8080, 10);
    accept_connections(listener_socket);

    close_logger();
    return 0;
}
