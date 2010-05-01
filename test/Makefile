CFLAGS = -I..

OBJS = main.o test.o testgen.o \
	   testrawbitget.o testrawbitcmp.o testrawbitrand.o \
	   testrawbitclear.o testrawbitset.o testmake.o
MAIN = main

all: $(OBJS)
	cd ..; make

clean:
	rm -rf $(MAIN) $(OBJS)

test: all
	$(CC) -o $(MAIN) $(OBJS) ../bitscan.o
	./$(MAIN)

