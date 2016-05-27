.PHONY:clean all
BIN = echoServer echoClient
OBJECTS = server.o 
CC = gcc 
GFLAGS = -Wall -g
all : $(BIN)
.o .c:
	$(CC) $(CFLAGS)  $< -o $@
clean :
	rm -f *.o $(BIN) 

