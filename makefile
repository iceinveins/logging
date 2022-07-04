CC=g++
CFLAGS=-I. -Wall -Wextra -lrt -pthread -march=nocona -mtune=generic

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

all: logserver logclient testserver
logserver: logserver.o shm.o
	$(CC) -o $@ $^ $(CFLAGS)

logclient: logclient.o shm.o
	$(CC) -o $@ $^ $(CFLAGS)

testserver: testserver.o shm.o
	$(CC) -o $@ $^ $(CFLAGS)


.PHONY: clean

clean:
	rm -f *.o *~ core *.log