CC = gcc
CFLAGS = -Wall
OBJECTS = main.o complex.o

output: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -lm -o output

main.o: main.c complex.h
	$(CC) $(CFLAGS) -c main.c

complex.o: complex.c complex.h
	$(CC) $(CFLAGS) -c complex.c

clean:
	rm -f output $(OBJECTS)
