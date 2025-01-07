#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>

#define WKP "/tmp/wkp"

void cleanup(int sig) {
    unlink(WKP); // Delete the WKP when the server exits
    exit(0);
}

int main() {
    signal(SIGINT, cleanup);  // Handle Ctrl-C to clean up
    int from_client, to_client;
    char buffer[100];
    srand(time(NULL));  // Initialize random number generator

    // Create a well-known pipe (WKP)
    if (mkfifo(WKP, 0666) == -1) {
        perror("mkfifo WKP");
        exit(1);
    }

    while (1) {
        // Wait for a client to connect
        printf("Server waiting for client connection...\n");
        from_client = open(WKP, O_RDONLY);
        if (from_client == -1) {
            perror("open WKP");
            exit(1);
        }

        // Handle the client
        int random_number = rand() % 101; // Generate a random number between 0 and 100
        snprintf(buffer, sizeof(buffer), "%d", random_number);

        // Send the random number to the client every second
        while (1) {
            write(from_client, buffer, strlen(buffer) + 1);
            printf("Sent: %s\n", buffer);
            sleep(1);  // Wait for 1 second before sending again
        }

        close(from_client);
    }

    return 0;
}
