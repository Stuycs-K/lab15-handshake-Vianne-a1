#include "pipe_networking.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void handle_sigint(int sig) {
    remove_fifo("wkp");
    printf("WKP removed and server shutting down.\n");
    exit(0);
}

int main() {
    // Handle SIGINT for graceful shutdown
    signal(SIGINT, handle_sigint);

    int to_client, from_client;

    // Create the Well Known Pipe (WKP) for clients to connect
    create_fifo("wkp");

    while (1) {
        // Server-side: Handle the handshake with a client
        server_handshake(&from_client);

        // Sleep for 1 second before accepting another client
        sleep(1);

        // Cleanup: Remove the WKP after communication ends
        remove_fifo("wkp");
        create_fifo("wkp");
    }

    return 0;
}
