CC=gcc
CFLAGS=-O3 -march=native -Wall
lc: lc.c
	$(CC) -o lc $(CFLAGS) $<

clean:
	$(RM) lc

