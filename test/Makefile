CFLAGS = -Wall -std=c99 -O2

OBJS = bitscan.o main.o test.o testgen.o \
	   testrawbitclear.o testrawbitcmp.o testrawbitcpy.o \
	   testrawbitget.o \
	   testrawbitop.o testrawbitrand.o \
	   testrawbitset.o testmake.o
MAIN = main

all: bitscan $(OBJS)
	cd ..; make

bitscan:
	cp ../bitscan.h ../bitscan.c .

clean:
	rm -rf $(MAIN) $(OBJS)

test: all
	$(CC) -o $(MAIN) $(CFLAGS) $(OBJS)
	./$(MAIN)

