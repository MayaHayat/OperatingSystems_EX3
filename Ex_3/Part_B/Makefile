CC = gcc
CFLAGS = -Wall -g
LIBRARY = libproactor.so

all: $(LIBRARY)

proactor.o: proactor.c proactor.h
	$(CC) $(CFLAGS) -c -fPIC proactor.c -o proactor.o

$(LIBRARY): proactor.o
	$(CC) -shared -o $(LIBRARY) proactor.o

clean:
	rm -f proactor.o $(LIBRARY)
