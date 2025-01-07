#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "pipe_networking.h"

int create_fifo(char *name) {
    // Create the named pipe (FIFO)
    if (mkfifo(name, 0666) == -1) {
        perror("mkfifo");
        return -1;
    }
    return 0;
}

int open_fifo(char *name, int flags) {
    // Open the named pipe (FIFO)
    int fd = open(name, flags);
    if (fd == -1) {
        perror("open");
        return -1;
    }
    return fd;
}

int server_handshake(char *wkp_name) {
    int from_client;

    // Open the WKP and wait for the client
    from_client = open_fifo(wkp_name, O_RDONLY);
    if (from_client == -1) {
        return -1;
    }

    // Read the client's private pipe name
    char client_pipe_name[256];
    read(from_client, client_pipe_name, sizeof(client_pipe_name));

    // Remove the WKP after accepting a client
    remove_fifo(wkp_name);

    // Create and open the client's private pipe for communication
    int to_client = open_fifo(client_pipe_name, O_WRONLY);
    if (to_client == -1) {
        return -1;
    }

    // Send the first message to the client (e.g., random number or handshake data)
    int random_number = rand() % 101;
    write(to_client, &random_number, sizeof(int));

    // Return the client file descriptor for communication
    return from_client;
}

void remove_fifo(char *name) {
    // Remove the named pipe (FIFO)
    if (unlink(name) == -1) {
        perror("unlink");
    }
}
