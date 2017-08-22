
CC = gcc
CFLAGS = -Wall -O2

.PHONY: clean

bin/archinfo32: archinfo.c
	$(CC) $(CFLAGS) -m32 archinfo.c -o ./bin/archinfo32

bin/archinfo64: archinfo.c
	$(CC) $(CFLAGS) -m64 archinfo.c -o ./bin/archinfo64

clean:
	rm -f ./bin/archinfo32 ./bin/archinfo64

