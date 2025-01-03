#include "pipe_networking.h"

/*=========================
  server_setup
  =========================*/
int server_setup() {
    int from_client;

    // Create the well-known pipe (WKP)
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
int server_handshake(int from_client, int *to_client) {
    char private_pipe[256];

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

    // Receive final acknowledgment from the client
    char ack_response[256];
    if (read(from_client, ack_response, sizeof(ack_response)) <= 0) {
        perror("read");
        exit(1);
    }

    if (strcmp(ack_response, "ACK") != 0) {
        fprintf(stderr, "[server] Handshake failed: invalid acknowledgment from client.\n");
        exit(1);
    }

    printf("[server] Handshake complete.\n");

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

    // Send final acknowledgment to the server
    if (write(*to_server, "ACK", 3) == -1) {
        perror("write");
        exit(1);
    }

    printf("[client] Handshake complete.\n");

    // Remove the private pipe
    if (remove(private_pipe) == -1) {
        perror("remove");
    }

    return from_server;
}

/*=========================
  multi_server_setup
  =========================*/
int multi_server_setup() {
    // Implementation for setting up a multi-server environment
    int from_client = server_setup();
    printf("[multi-server] Multi-server setup complete.\n");
    return from_client;
}

/*=========================
  multi_server_connect
  =========================*/
int multi_server_connect(int from_client, struct message m) {
    // Implementation for connecting clients in a multi-server setup
    printf("[multi-server] Received message type: %d, data: %s\n", m.type, m.data);
    return 0; // Placeholder for actual connection logic
}
