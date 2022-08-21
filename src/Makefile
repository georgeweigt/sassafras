.PHONY: clean
CC = gcc
CFLAGS = -O0

objs := $(shell ls *.c | sed "s/\.c/\.o/g;s/emit_line\.o//")

sassafras: $(objs) defs.h prototypes.h
	gcc -o sassafras $(objs) -lm

clean:
	rm -f sassafras *.o
