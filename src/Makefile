.PHONY: clean
CC = gcc
CFLAGS = -O0

objs := $(shell ls *.c | sed "s/\.c/\.o/g;s/emit_line\.o//")

sassafras: $(objs)
	gcc -o sassafras $(objs) -lm

$(objs): defs.h prototypes.h

clean:
	rm -f sassafras *.o
