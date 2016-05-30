.PHONY:clean all
BIN = p2pServer p2pClient selectP2pCli  selectP2pSer
OBJECTS = server.o 
CC = gcc 
GFLAGS = -Wall -g
all : $(BIN)
.o .c:
	$(CC) $(CFLAGS)  $< -o $@
clean :
	rm -f *.o $(BIN) 

