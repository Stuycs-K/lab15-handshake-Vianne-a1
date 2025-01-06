#include "pipe_networking.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int to_server, from_server;

    // Client-side: Perform the handshake with the server
    from_server = client_handshake(&to_server);

    // Keep receiving data from the server
    while (1) {
        int x;
        read(from_server, &x, sizeof(x));
        printf("Client received: %d\n", x);
        sleep(1);  // Sleep for 1 second before requesting more data
    }

    close(from_server);
    return 0;
}
