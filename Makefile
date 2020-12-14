
all: lc.c
	gcc lc.c -o lc -O3 -march=native

clean:
	$(RM) lc

