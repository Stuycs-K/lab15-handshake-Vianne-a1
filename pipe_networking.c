#include "pipe_networking.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>

#define WKP_NAME "wkp"  // Well Known Pipe (WKP)

// Function to create a named FIFO (pipe)
int create_fifo(const char *name) {
    if (mkfifo(name, 0666) == -1) {
        perror("Error creating FIFO");
        exit(1);
    }
    return 0;
}

// Function to remove a named FIFO (pipe)
void remove_fifo(const char *name) {
    if (remove(name) == -1) {
        perror("Error removing FIFO");
        exit(1);
    }
}

// Client handshake
int client_handshake(int *to_server) {
    char pipe_name[100];
    int pid = getpid();

    // Create a private FIFO (client's pipe) using the client's PID as the name
    sprintf(pipe_name, "%d", pid);
    create_fifo(pipe_name);

    // Connect to the server's Well Known Pipe (WKP)
    *to_server = open(WKP_NAME, O_WRONLY);
    if (*to_server == -1) {
        perror("Error opening WKP");
        exit(1);
    }

    // Send the client's PID to the server
    write(*to_server, &pid, sizeof(pid));
    close(*to_server);

    // Wait for the server to send data via the private pipe
    int from_server = open(pipe_name, O_RDONLY);
    if (from_server == -1) {
        perror("Error opening client's private pipe");
        exit(1);
    }

    // Read the random number or data from the server
    int x;
    read(from_server, &x, sizeof(x));
    printf("Client received: %d\n", x);

    close(from_server);
    remove_fifo(pipe_name);  // Clean up the private pipe
    return 0;
}

// Server handshake half
int server_handshake_half(int *to_client, int from_client) {
    char pipe_name[100];

    // Get the client's PID from the message
    int pid;
    read(from_client, &pid, sizeof(pid));

    // Create the private FIFO for the client
    sprintf(pipe_name, "%d", pid);
    create_fifo(pipe_name);

    // Open the private pipe to send a response back to the client
    *to_client = open(pipe_name, O_WRONLY);
    if (*to_client == -1) {
        perror("Error opening client's private pipe");
        exit(1);
    }

    // Send a random number to the client
    int x = rand() % 101;  // Random number between 0 and 100
    write(*to_client, &x, sizeof(x));
    close(*to_client);

    return 0;
}

// Server handshake
int server_handshake(int *to_client) {
    int from_client;

    // Open the Well Known Pipe (WKP) and wait for a client to connect
    from_client = open(WKP_NAME, O_RDONLY);
    if (from_client == -1) {
        perror("Error opening WKP");
        exit(1);
    }

    // Remove the WKP after connection is established
    remove_fifo(WKP_NAME);

    // Complete the handshake with the client
    server_handshake_half(to_client, from_client);

    close(from_client);  // Close the connection
    return 0;
}

// Signal handler for SIGINT to clean up WKP
void handle_sigint(int sig) {
    remove_fifo(WKP_NAME);
    printf("WKP removed and server shutting down.\n");
    exit(0);
}

int main() {
    // Handle SIGINT for graceful shutdown
    signal(SIGINT, handle_sigint);

    int to_client, from_client;

    // Create the Well Known Pipe (WKP) for clients to connect
    create_fifo(WKP_NAME);

    // Server-side: Handle the handshake with a client
    server_handshake(&from_client);

    // Cleanup: Remove the WKP after communication ends
    remove_fifo(WKP_NAME);

    return 0;
}
