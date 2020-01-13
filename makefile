CC=gcc

main: main.c complex.c
	$(CC) -o output main.c complex.c -lm
