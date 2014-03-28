CC = gcc
CFLAGS = -g -DRPC_SVC_FG

all: mapred

mapred: mapred.o
	$(CC) -o mapred mapred.o

mapred.o: mapred.c mapred.h
	$(CC) $(CFLAGS) -c mapred.c

clean:
	rm -f mapred mapred.o
