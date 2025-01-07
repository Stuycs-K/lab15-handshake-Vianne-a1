#include "pipe_networking.h"
#include <signal.h>

int main() {
    signal(SIGINT, handle_sigint);

    while (1) {
        int from_client = open(WKP, O_RDONLY);
        if (from_client == -1) {
            perror("Error opening WKP");
            exit(1);
        }

        pid_t pid = fork();
        if (pid == 0) { // Child process
            int to_client;
            server_handshake_half(&to_client, from_client);

            // Communication loop
            while (1) {
                int x = rand() % 101;
                write(to_client, &x, sizeof(x));
                sleep(1);
            }

            close(from_client);
            close(to_client);
            exit(0);
        } else if (pid > 0) { // Parent process
            close(from_client);
        } else {
            perror("Fork failed");
            exit(1);
        }
    }

    return 0;
}
