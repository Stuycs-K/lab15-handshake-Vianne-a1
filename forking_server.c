#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <string.h>
#include <time.h>

#define WKP "/tmp/wkp"

void cleanup(int sig) {
    unlink(WKP); // Delete the WKP when the server exits
    exit(0);
}

void handle_client(int from_client) {
    srand(time(NULL));
    char buffer[100];
    int random_number = rand() % 101;

    snprintf(buffer, sizeof(buffer), "%d", random_number);
    while (1) {
        write(from_client, buffer, strlen(buffer) + 1); // Send the number
        printf("Sent to client: %s\n", buffer);
        sleep(1);
    }
}

int main() {
    signal(SIGINT, cleanup);

    // Create a well-known pipe (WKP)
    if (mkfifo(WKP, 0666) == -1) {
        perror("mkfifo WKP");
        exit(1);
    }

    while (1) {
        // Wait for a connection from the client
        int from_client = open(WKP, O_RDONLY);
        if (from_client == -1) {
            perror("open WKP");
            exit(1);
        }

        // Fork to handle the client in a separate process
        pid_t pid = fork();
        if (pid == 0) {
            // Child process handles the client
            close(from_client);  // Close the file descriptor in the parent
            handle_client(from_client);
            exit(0);
        } else {
            close(from_client);  // Close the file descriptor in the parent
        }
    }

    return 0;
}
