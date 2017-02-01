CC=gcc
CFLAGS=--std=c11 -Wall -Wextra -Werror -g3

all: build

build: functions.o equations.o test.o
	$(CC) -Wall -Werror -Wextra -lm functions.o equations.o test.o --std=c11 -g3 -o test

library: test.o
	$(CC) -Wall -Werror -Wextra -lm -fPIC functions.c equations.c -shared -o libequation.so
	$(CC) -Wall -Werror -Wextra -L. -lequation test.o --std=c11 -o test

clean:
	rm -rvf *.o
	rm -rvf *.so
	rm -rvf test
