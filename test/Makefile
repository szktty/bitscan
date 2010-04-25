CFLAGS = -I..

OBJS = main.o test.o testmake.o
MAIN = main

all: $(OBJS)
	cd ..; make

test: all
	$(CC) -o $(MAIN) $(OBJS) ../bitscan.o
	./$(MAIN)

