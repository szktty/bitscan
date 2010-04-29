CFLAGS = -I..

OBJS = main.o test.o testrawbitcmp.o testmake.o
MAIN = main

all: $(OBJS)
	cd ..; make

clean:
	rm -rf $(MAIN) $(OBJS)

test: all
	$(CC) -o $(MAIN) $(OBJS) ../bitscan.o
	./$(MAIN)

