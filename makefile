CC = gcc
CFLAGS = -Wall -Wextra -g
TARGETS = basic_client basic_server persistant_server forking_server

all: $(TARGETS)

client: basic_client.c pipe_networking.c
	$(CC) $(CFLAGS) -o client basic_client.c pipe_networking.c

server: basic_server.c pipe_networking.c
	$(CC) $(CFLAGS) -o server basic_server.c pipe_networking.c

persistant_server: persistant_server.c pipe_networking.c
	$(CC) $(CFLAGS) -o persistant_server persistant_server.c pipe_networking.c

forking_server: forking_server.c pipe_networking.c
	$(CC) $(CFLAGS) -o forking_server forking_server.c pipe_networking.c

clean:
	rm -f $(TARGETS)
	rm -f *.o
	rm -f wkp
