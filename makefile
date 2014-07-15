CC=clang
CFLAGS=-g -O0
VPATH=src

all: example

example: example.c cutl.o

clean:
	rm -rf *.o example
	