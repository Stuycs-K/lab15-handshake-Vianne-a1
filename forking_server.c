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

void server_handshake_half(int to_client, int from_client) {
    // Complete the handshake and communicate with the client.
    int x = rand() % 101;
    write(to_client, &x, sizeof(x));  // Send a random integer to the client.
}

void server_setup() {
    int from_client;
    mkfifo(WKP, 0666);  // Create the Well Known Pipe (WKP).
    from_client = open(WKP, O_RDONLY);  // Wait for a client to connect.
    if (from_client == -1) {
        perror("Error accepting connection");
        return;
    }

    int pid;
    read(from_client, &pid, sizeof(pid));  // Read client's private pipe name (client PID).
    printf("Received client PID: %d\n", pid);
    remove(WKP);  // Remove WKP once the connection is established.
    
    // Now fork to handle the client in a separate process.
    pid_t child_pid = fork();
    if (child_pid == 0) {
        // In child process (subserver)
        int to_client = open("client_pipe", O_WRONLY);  // Open the client's private pipe.
        server_handshake_half(to_client, from_client);
        close(to_client);
        close(from_client);
        exit(0);  // Exit the subserver process.
    }

    close(from_client);
}

int main() {
    signal(SIGINT, handle_sigint);

    while (1) {
        server_setup();  // Accept connections and fork a subserver for each client.
    }

    return 0;
}
