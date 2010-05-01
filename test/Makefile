CFLAGS = -I..

OBJS = main.o test.o testgen.o \
	   testrawbitclear.o testrawbitcmp.o testrawbitcpy.o \
	   testrawbitget.o \
	   testrawbitrand.o \
	   testrawbitset.o testmake.o
MAIN = main

all: $(OBJS)
	cd ..; make

clean:
	rm -rf $(MAIN) $(OBJS)

test: all
	$(CC) -o $(MAIN) $(OBJS) ../bitscan.o
	./$(MAIN)

