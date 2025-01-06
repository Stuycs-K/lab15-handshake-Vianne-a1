#ifndef PIPE_NETWORKING_H
#define PIPE_NETWORKING_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

// Client and server handshake functions
int client_handshake(int *to_server);
int server_handshake(int *to_client);

// Helper functions for creating and removing pipes
int create_fifo(const char *name);
void remove_fifo(const char *name);

#endif
