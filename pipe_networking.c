#include "pipe_networking.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

/*=========================
  server_setup
  =========================*/
int server_setup() {
  int from_client;

  // Create the well-known pipe
  if (mkfifo("WKP", 0644) == -1) {
    perror("mkfifo");
    exit(1);
  }

  // Open the WKP for reading
  from_client = open("WKP", O_RDONLY);
  if (from_client == -1) {
    perror("open");
    exit(1);
  }

  // Remove the WKP after connection
  if (remove("WKP") == -1) {
    perror("remove");
  }

  return from_client;
}

/*=========================
  server_handshake
  =========================*/
int server_handshake(int *to_client) {
  char private_pipe[100];
  int from_client;

  // Read the name of the private pipe from the client
  if (read(from_client, private_pipe, sizeof(private_pipe)) == -1) {
    perror("read");
    exit(1);
  }

  // Open the private pipe for writing
  *to_client = open(private_pipe, O_WRONLY);
  if (*to_client == -1) {
    perror("open");
    exit(1);
  }

  // Send acknowledgment to the client
  if (write(*to_client, "ACK", 3) == -1) {
    perror("write");
    exit(1);
  }

  // Receive final acknowledgment from the client
  char ack_response[100];
  if (read(from_client, ack_response, sizeof(ack_response)) == -1) {
    perror("read");
    exit(1);
  }

  if (strcmp(ack_response, "ACK") != 0) {
    fprintf(stderr, "Handshake failed: invalid acknowledgment from client.\n");
    exit(1);
  }

  return from_client;
}

/*=========================
  client_handshake
  =========================*/
int client_handshake(int *to_server) {
  int to_server_fd, from_server_fd;
  char private_pipe[100];

  // Generate private pipe name
  sprintf(private_pipe, "private_%d", getpid());

  // Create the private pipe
  if (mkfifo(private_pipe, 0644) == -1) {
    perror("mkfifo");
    exit(1);
  }

  // Open the well-known pipe for writing
  *to_server = open("WKP", O_WRONLY);
  if (*to_server == -1) {
    perror("open");
    exit(1);
  }

  // Send the private pipe name to the server
  if (write(*to_server, private_pipe, strlen(private_pipe) + 1) == -1) {
    perror("write");
    exit(1);
  }

  // Open the private pipe for reading
  from_server_fd = open(private_pipe, O_RDONLY);
  if (from_server_fd == -1) {
    perror("open");
    exit(1);
  }

  // Read the server's acknowledgment
  char ack[100];
  if (read(from_server_fd, ack, sizeof(ack)) == -1) {
    perror("read");
    exit(1);
  }

  if (strcmp(ack, "ACK") != 0) {
    fprintf(stderr, "Handshake failed: invalid acknowledgment from server.\n");
    exit(1);
  }

  // Send final acknowledgment to the server
  if (write(*to_server, "ACK", 3) == -1) {
    perror("write");
    exit(1);
  }

  // Remove the private pipe
  if (remove(private_pipe) == -1) {
    perror("remove");
  }

  return from_server_fd;
}

/*=========================
  server_connect
  =========================*/
int server_connect(int from_client) {
  char private_pipe[100];

  // Read the private pipe name from the client
  if (read(from_client, private_pipe, sizeof(private_pipe)) == -1) {
    perror("read");
    exit(1);
  }

  // Open the private pipe for writing
  int to_client = open(private_pipe, O_WRONLY);
  if (to_client == -1) {
    perror("open");
    exit(1);
  }

  return to_client;
}