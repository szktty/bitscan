CFLAGS = -Wall -std=c99 -O2

OBJS = bitscan.o main.o test.o testgen.o \
	   testrawbitclear.o testrawbitcmp.o testrawbitcpy.o \
	   testrawbitget.o \
	   testrawbitop.o testrawbitrand.o \
	   testrawbitset.o testrawbitshift.o testmake.o
MAIN = main

all: test

bitscan:
	cp ../bitscan.h ../bitscan.c .

test: bitscan $(OBJS)
	$(CC) -o $(MAIN) $(CFLAGS) $(OBJS)
	./$(MAIN)

clean:
	rm -rf $(MAIN) $(OBJS)

