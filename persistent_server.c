#include "pipe_networking.h"

int main() {
    signal(SIGINT, handle_sigint);

    while (1) {
        int to_client, from_client;

        // Wait for a client to connect
        from_client = server_handshake(&to_client);

        // Handle client communication
        int x = rand() % 101;
        write(to_client, &x, sizeof(x));

        close(from_client);
        close(to_client);

        printf("Client disconnected. Ready for a new client.\n");
    }

    return 0;
}
