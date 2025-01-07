#ifndef PIPE_NETWORKING_H
#define PIPE_NETWORKING_H

// Function declarations for pipe networking
int create_fifo(char *name);
int open_fifo(char *name, int flags);
int server_handshake(char *wkp_name);
void remove_fifo(char *name);

#endif
