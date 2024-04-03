SHELL := /bin/sh
CC = clang
#CFLAGS = -Wall -g -Wpedantic -Werror -Wextra -Wstrict-prototypes $(shell pkg-config --cflags gmp) -gdwarf-4
#LFLAGS = $(shell pkg-config --libs gmp)
CFLAGS = -Wall -g -Wpedantic -Werror -Wextra -Wstrict-prototypes -I$(shell brew --prefix gmp)/include -gdwarf-4
LFLAGS = -L$(shell brew --prefix gmp)/lib -lgmp

all: encrypt decrypt keygen

encrypt: encrypt.o ss.o numtheory.o randstate.o
	$(CC) -o $@ $^ $(LFLAGS)

decrypt: decrypt.o randstate.o ss.o numtheory.o
	$(CC) -o $@ $^ $(LFLAGS)

keygen: keygen.o randstate.o ss.o numtheory.o
	$(CC) -o $@ $^ $(LFLAGS)

encrypt.o: encrypt.c ss.o numtheory.o randstate.o
	$(CC) $(CFLAGS) -c $<

decrypt.o: decrypt.c randstate.o ss.o numtheory.o
	$(CC) $(CFLAGS) -c $<

keygen.o: keygen.c randstate.o ss.o numtheory.o
	$(CC) $(CFLAGS) -c $<

#scan-build:
#	 --use-cc=clang -no-failure-reports --status-bugs
#scan-build: clean
#	scan-build --use-cc=$(CC) make

clean:
	rm -f encrypt decrypt keygen *.o

format:
	clang-format -i -style=file *.[ch]


