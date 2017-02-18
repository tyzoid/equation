CC=gcc
CFLAGS=--std=c11 -Wall -Wextra -Werror -g3

all: build

build: functions.o equations.o test.o
	$(CC) $(CFLAGS) -lm functions.o equations.o test.o -o test

library: test.o
	$(CC) $(CFLAGS) -lm -fPIC functions.c equations.c -shared -o libequation.so
	$(CC) $(CFLAGS) -L. -lequation test.o --std=c11 -o test

clean:
	rm -rvf *.o
	rm -rvf *.so
	rm -rvf test
