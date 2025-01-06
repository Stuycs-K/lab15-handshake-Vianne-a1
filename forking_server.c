#include "pipe_networking.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
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
    pid_t pid;

    // Create the Well Known Pipe (WKP) for clients to connect
    create_fifo("wkp");

    while (1) {
        // Wait for a client to connect on the WKP
        from_client = open("wkp", O_RDONLY);
        if (from_client == -1) {
            perror("Error opening WKP");
            exit(1);
        }

        // Create a child process for handling the client
        pid = fork();

        if (pid == 0) {  // Child process
            // Complete the handshake
            server_handshake_half(&to_client, from_client);

            // Send random integers to client repeatedly
            while (1) {
                int x = rand() % 101;
                write(to_client, &x, sizeof(x));
                sleep(1);
            }

            close(to_client);
            exit(0);
        } else if (pid > 0) {
            // Parent process: Close the from_client file descriptor and continue to wait for new clients
            close(from_client);
        } else {
            perror("Fork failed");
            exit(1);
        }

        // Clean up and recreate the WKP after each client disconnect
        remove_fifo("wkp");
        create_fifo("wkp");
    }

    return 0;
}
