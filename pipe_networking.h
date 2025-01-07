#ifndef PIPE_NETWORKING_H
#define PIPE_NETWORKING_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>

#define BUFFER_SIZE 256 // Define the size of the buffer
#define WKP "wkp"       // Define the name of the well-known pipe

// Function prototypes
int server_handshake(int *to_client);
int client_handshake(int *to_server);
void handle_sigint(int sig);

#endif