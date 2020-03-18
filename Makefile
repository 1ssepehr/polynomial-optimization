CC = gcc
CFLAGS = -Wall

genetic bitwise: bitwise.o genetic.o complex.o
	$(CC) $(CFLAGS) bitwise.o complex.o -lm -o bitwise
	$(CC) $(CFLAGS) genetic.o complex.o -lm -o genetic

bitwise.o: bitwise.c bitwise.h complex.h
	$(CC) $(CFLAGS) -c bitwise.c

genetic.o: genetic.c complex.h
	$(CC) $(CFLAGS) -c genetic.c

complex.o: complex.c complex.h
	$(CC) $(CFLAGS) -c complex.c

clean:
	rm -f genetic bitwise genetic.o bitwise.o complex.o
