IDIR= ./include
CC= g++
CFLAGS= -I$(IDIR) -Wall -Wextra

ODIR= ./obj

LIBS=-lrt

_DEPS= ipc.h shm.h
DEPS= $(patsubst %,$(IDIR)/%, $(_DEPS))

_OBJ= shm.o
OBJ= $(patsubst %,$(ODIR)/%, $(_OBJ))

$(ODIR)/%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS)

all: logserver.elf logclient.elf

logserver.elf: $(OBJ) $(ODIR)/logserver.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

logclient.elf: $(OBJ) $(ODIR)/logclient.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)


.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core *.elf *.sock ./domainsocket *.log