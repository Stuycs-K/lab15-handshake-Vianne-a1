#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_BUF 100

int main() {
    int client_fd;
    char *fifo_name = "/tmp/client_fifo";

    // Create the private pipe
    if (mkfifo(fifo_name, 0666) == -1) {
        perror("mkfifo");
        exit(1);
    }

    // Open the private pipe for reading
    client_fd = open(fifo_name, O_RDONLY);
    if (client_fd == -1) {
        perror("open fifo");
        exit(1);
    }

    char buffer[MAX_BUF];

    // Receive and print messages from the server
    while (1) {
        ssize_t bytes_read = read(client_fd, buffer, MAX_BUF);
        if (bytes_read == -1) {
            perror("read");
            break;
        }
        if (bytes_read == 0) {
            break;
        }

        buffer[bytes_read] = '\0';  // Null terminate the received message
        printf("Received from server: %s\n", buffer);
        sleep(1); // Wait for 1 second
    }

    close(client_fd);
    unlink(fifo_name); // Delete the private FIFO
    return 0;
}
