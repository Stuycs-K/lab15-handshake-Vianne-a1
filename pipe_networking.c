#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include "pipe_networking.h"

/*=========================
  server_setup
  =========================*/
int server_setup() {
    int from_client;

    // Create the well-known pipe (WKP) if it doesn't exist
    if (mkfifo(WKP, 0644) == -1) {
        if (errno != EEXIST) {
            perror("mkfifo");
            exit(1);
        }
    }

    // Open the WKP for reading
    from_client = open(WKP, O_RDONLY);
    if (from_client == -1) {
        perror("open");
        exit(1);
    }

    printf("[server] WKP created and waiting for connection...\n");

    return from_client;
}

/*=========================
  server_handshake
  =========================*/
int server_handshake(int *to_client) {
    int from_client;
    char private_pipe[256];

    // Open the WKP for reading (ensure WKP exists)
    from_client = open(WKP, O_RDONLY);
    if (from_client == -1) {
        perror("open");
        exit(1);
    }

    printf("[server] Waiting for client connection...\n");

    // Read the name of the private pipe from the client
    if (read(from_client, private_pipe, sizeof(private_pipe)) <= 0) {
        perror("read");
        exit(1);
    }

    printf("[server] Received private pipe name: %s\n", private_pipe);

    // Open the private pipe for writing
    *to_client = open(private_pipe, O_WRONLY);
    if (*to_client == -1) {
        perror("open");
        exit(1);
    }

    // Send acknowledgment to the client
    if (write(*to_client, "ACK", 3) == -1) {
        perror("write");
        exit(1);
    }

    printf("[server] Sent ACK to client.\n");

    return from_client;
}

/*=========================
  client_handshake
  =========================*/
int client_handshake(int *to_server) {
    int from_server;
    char private_pipe[256];

    // Generate private pipe name
    sprintf(private_pipe, "private_%d", getpid());

    // Create the private pipe
    if (mkfifo(private_pipe, 0644) == -1) {
        perror("mkfifo");
        exit(1);
    }

    // Open the well-known pipe for writing
    *to_server = open(WKP, O_WRONLY);
    if (*to_server == -1) {
        perror("open");
        exit(1);
    }

    // Send the private pipe name to the server
    if (write(*to_server, private_pipe, strlen(private_pipe) + 1) == -1) {
        perror("write");
        exit(1);
    }

    printf("[client] Sent private pipe name to server.\n");

    // Open the private pipe for reading
    from_server = open(private_pipe, O_RDONLY);
    if (from_server == -1) {
        perror("open");
        exit(1);
    }

    // Read the server's acknowledgment
    char ack[256];
    if (read(from_server, ack, sizeof(ack)) <= 0) {
        perror("read");
        exit(1);
    }

    if (strcmp(ack, "ACK") != 0) {
        fprintf(stderr, "[client] Handshake failed: invalid acknowledgment from server.\n");
        exit(1);
    }

    printf("[client] Received ACK from server.\n");

    return from_server;
}

/*=========================
  server_connect
  =========================*/
int server_connect(int from_client) {
    char buffer[BUFFER_SIZE];

    // Example of handling a single client connection
    if (read(from_client, buffer, sizeof(buffer)) > 0) {
        printf("[server] Received: %s\n", buffer);

        // Here, handle the different message types
        if (strncmp(buffer, "EXIT", 4) == 0) {
            printf("[server] Client requested exit.\n");
            return -1; // Exit the connection
        }
    }

    perror("read");
    return 0; // Continue waiting for messages
}

/*=========================
  multi_server_setup
  =========================*/
int multi_server_setup() {
    int from_client = server_setup();
    printf("[multi-server] Multi-server setup complete.\n");
    return from_client;
}

/*=========================
  multi_server_connect
  =========================*/
int multi_server_connect(int from_client, struct message m) {
    // Use 'from_client' if necessary or remove it if not needed.
    (void) from_client;  // Prevent unused parameter warning

    // Handle different message types
    printf("[multi-server] Received message type: %d, data: %s\n", m.type, m.data);

    // Respond based on message type
    switch (m.type) {
        case MESSAGE:
            printf("[multi-server] Message: %s\n", m.data);
            break;
        case EXIT:
            printf("[multi-server] Client requested exit.\n");
            return -1; // Close the connection
        default:
            printf("[multi-server] Unknown message type: %d\n", m.type);
    }

    return 0; // Continue communication
}
