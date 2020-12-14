CC=gcc
CFLAGS=-O3 -march=native -Wall -Wno-stringop-truncation
lc: lc.c
	$(CC) -o lc $(CFLAGS) $<

clean:
	$(RM) lc

