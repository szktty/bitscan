CFLAGS = -Wall -std=c99 -O2

OBJS = bitscan.o main.o test.o testgen.o \
	   testbitclear.o testbitcmp.o testbitcpy.o \
	   testbitget.o \
	   testbitop.o testbitrand.o \
	   testbitset.o testbitshift.o
MAIN = main

all: test

bitscan:
	cp ../bitscan.h ../bitscan.c .

test: bitscan $(OBJS)
	$(CC) -o $(MAIN) $(CFLAGS) $(OBJS)
	./$(MAIN)

clean:
	rm -rf $(MAIN) $(OBJS)

