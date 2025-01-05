CC = gcc
CFLAGS = -Wall -g

all: compile

compile:
	$(CC) $(CFLAGS) -o server basic_server.c
	$(CC) $(CFLAGS) -o client basic_client.c

server:
	$(CC) $(CFLAGS) -o forking_server forking_server.c

client:
	$(CC) $(CFLAGS) -o client client.c

clean:
	rm -f server client forking_server basic_server basic_client wkp client_pipe
