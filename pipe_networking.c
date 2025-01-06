#ifndef PIPE_NETWORKING_H
#define PIPE_NETWORKING_H

#define WKP "mario"               // Well Known Pipe
#define BUFFER_SIZE 1000
#define MAX_CLIENTS 100

#define SYN 0
#define SYN_ACK 1
#define ACK 2
#define MESSAGE 3
#define EXIT 4

// Message structure for communication
struct message {
    int type;
    char data[BUFFER_SIZE];
};

// Function declarations
int server_handshake(int *to_client);
int client_handshake(int *to_server);

int server_connect(int from_client);

int server_setup();
int multi_server_setup();

int multi_server_connect(int from_client, struct message m);

#endif