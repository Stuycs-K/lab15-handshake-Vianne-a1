# Makefile for pipe networking project

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Targets
all: compile

compile: client server

client: basic_client.o pipe_networking.o
	$(CC) -o client basic_client.o pipe_networking.o

server: basic_server.o pipe_networking.o
	$(CC) -o server basic_server.o pipe_networking.o

basic_client.o: basic_client.c pipe_networking.h
	$(CC) $(CFLAGS) -c basic_client.c

basic_server.o: basic_server.c pipe_networking.h
	$(CC) $(CFLAGS) -c basic_server.c

pipe_networking.o: pipe_networking.c pipe_networking.h
	$(CC) $(CFLAGS) -c pipe_networking.c

clean:
	rm -f *.o client server
	rm -f *~
