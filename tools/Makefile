.PHONY: default clean

default:
	make -s a.out
	./a.out ../src/ >../src/prototypes.h

a.out: make-prototypes.c
	gcc make-prototypes.c

clean:
	rm -f a.out
