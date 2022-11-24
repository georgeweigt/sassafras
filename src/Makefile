.PHONY: clean

sassafras: *.h *.c
	gcc -Wall -O0 -o sassafras *.c -lm

prototypes.h: *.c
	make -s -C ../tools make-prototypes
	../tools/make-prototypes . > prototypes.h

clean:
	rm -f sassafras
