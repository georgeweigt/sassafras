.PHONY: all test

sassafras: sassafras.c
	$(CC) -Wall -O0 -o sassafras sassafras.c -lm

sassafras.c: src/LICENSE src/defs.h src/prototypes.h src/*.c
	cat src/LICENSE src/defs.h src/prototypes.h src/*.c > sassafras.c

all:
	make -C doc
	make -C src
	make sassafras

test:
	cd test; for FILE in $$(basename -a -s .in *.in); do echo $$FILE; ../sassafras $$FILE.in | diff - $$FILE.out; done
