#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define WKP "wkp"

void handle_sigint(int sig) {
    remove(WKP);  // Remove the WKP before exiting.
    printf("\nServer exiting gracefully...\n");
    exit(0);
}

int main() {
    signal(SIGINT, handle_sigint);
    mkfifo(WKP, 0666);  // Create the Well Known Pipe (WKP).

    while (1) {
        printf("Server waiting for a client...\n");

        int from_client = open(WKP, O_RDONLY);  // Wait for a client to connect.

        if (from_client == -1) {
            perror("Error accepting connection");
            continue;
        }

        // Handle the client (simple communication example):
        int x = rand() % 101;
        printf("Sending random number: %d\n", x);
        // Send the number to the client (using a private pipe or some other communication).

        close(from_client);  // Close the connection after sending the data.
    }

    return 0;
}
