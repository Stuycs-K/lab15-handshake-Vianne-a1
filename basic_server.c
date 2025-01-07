#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "pipe_networking.h"

#define WKP_NAME "wkp"  // Well-Known Pipe name

// SIGINT handler to clean up WKP
void handle_sigint(int sig) {
    remove_fifo(WKP_NAME);  // Remove WKP on exit
    printf("Server exiting...\n");
    exit(0);
}

int main() {
    signal(SIGINT, handle_sigint);  // Set up signal handler for cleanup

    // Create the Well-Known Pipe (WKP)
    create_fifo(WKP_NAME);

    while (1) {
        // Wait for client connection
        int from_client = server_handshake(WKP_NAME);
        if (from_client == -1) {
            perror("Error during handshake");
            continue;
        }

        // Handle communication with client
        int random_number = rand() % 101;  // Random number between 0 and 100
        while (1) {
            write(from_client, &random_number, sizeof(int));  // Send random number to client
            sleep(1);  // Sleep for 1 second before sending again
        }

        close(from_client);  // Close client connection after communication
    }

    return 0;
}
