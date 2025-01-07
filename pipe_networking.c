#include "pipe_networking.h"

int create_fifo(const char *name) {
    if (mkfifo(name, 0666) == -1) {
        perror("Error creating FIFO");
        return -1;
    }
    return 0;
}

void remove_fifo(const char *name) {
    if (unlink(name) == -1) {
        perror("Error removing FIFO");
    }
}

int server_handshake(int *to_client) {
    char buffer[BUFFER_SIZE];

    // Open WKP and wait for the client to connect
    int from_client = open(WKP, O_RDONLY);
    if (from_client == -1) {
        perror("Error opening WKP");
        exit(1);
    }

    // Read private pipe name from client
    if (read(from_client, buffer, BUFFER_SIZE) <= 0) {
        perror("Error reading from WKP");
        exit(1);
    }

    // Remove WKP after reading
    remove_fifo(WKP);

    // Open private pipe and send SYN_ACK
    *to_client = open(buffer, O_WRONLY);
    if (*to_client == -1) {
        perror("Error opening private pipe");
        exit(1);
    }

    int secret = rand() % 1000; // Random value for handshake
    write(*to_client, &secret, sizeof(secret));

    // Read ACK from client
    int ack;
    if (read(from_client, &ack, sizeof(ack)) <= 0 || ack != secret + 1) {
        perror("Error in handshake ACK");
        exit(1);
    }

    printf("Handshake complete.\n");
    return from_client;
}

int client_handshake(int *to_server) {
    char private_pipe[BUFFER_SIZE];
    sprintf(private_pipe, "%d", getpid());

    // Create private pipe
    if (create_fifo(private_pipe) == -1) {
        perror("Error creating private pipe");
        exit(1);
    }

    // Connect to WKP
    *to_server = open(WKP, O_WRONLY);
    if (*to_server == -1) {
        perror("Error opening WKP");
        exit(1);
    }

    // Send private pipe name to server
    write(*to_server, private_pipe, BUFFER_SIZE);

    // Open private pipe and wait for SYN_ACK
    int from_server = open(private_pipe, O_RDONLY);
    if (from_server == -1) {
        perror("Error opening private pipe");
        exit(1);
    }

    int secret;
    if (read(from_server, &secret, sizeof(secret)) <= 0) {
        perror("Error reading SYN_ACK");
        exit(1);
    }

    // Send ACK to server
    int ack = secret + 1;
    write(*to_server, &ack, sizeof(ack));

    // Remove private pipe
    remove_fifo(private_pipe);

    printf("Handshake complete.\n");
    return from_server;
}
