
CC=gcc
# If we build with ddc
CFLAGS=-Og -g -march=native -std=c11 -Wall -Wno-stringop-truncation -I. -lddcutil
# else
# CFLAGS=-Og -g -march=native -std=c11 -Wall -Wno-stringop-truncation -I.

LIBS = lclib.h
OBJS = lc.o lclib.o
MAIN = lc.c


%.o: %.c $(LIBS) #libs = objects confirmed
	$(CC) -c -o $@ $(CFLAGS) $< 

lc: $(OBJS)
	$(CC) -o lc -I $(CFLAGS) $^

list_displays: lc
	# saves a cmd
	# Can be removed in the future
	./lc -l

debug-lc: lc.c
	$(CC) -o lc-debug -Og -g $<

check:
	echo "check"
	valgrind --leak-check=full ./lc --help

suid: lc
	# I wouldn't use this willy-nilly
	sudo chown 0:0 ./lc
	sudo chmod +s ./lc

clean:
	$(RM) lc
	$(RM) $(OBJS)

