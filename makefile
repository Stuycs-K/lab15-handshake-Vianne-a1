all: client server

compile: basic_client.o pipe_networking.o forking_server.o pipe_networking.o
	gcc -o CLIENT basic_client.o pipe_networking.o
	gcc -o SERVER forking_server.o pipe_networking.o


server: SERVER
	./SERVER

client: CLIENT
	./CLIENT

basic_client.o: basic_client.c pipe_networking.h
	gcc -c basic_client.c

basic_server.o: basic_server.c pipe_networking.h
	gcc -c basic_server.c

persistant_server.o: persistant_server.c pipe_networking.h
	gcc -c persistant_server.c

forking_server.o: forking_server.c pipe_networking.h
	gcc -c forking_server.c 


pipe_networking.o: pipe_networking.c pipe_networking.h
	gcc -c pipe_networking.c

clean:
	rm SERVER
	rm CLIENT
	rm *.o
	rm *~
