.PHONY: default clean all test

default:
	make -C src prototypes.h
	make sassafras

sassafras: sassafras.c
	$(CC) -Wall -O0 -o sassafras sassafras.c -lm

sassafras.c: src/LICENSE src/defs.h src/prototypes.h src/*.c
	cat src/LICENSE src/defs.h src/prototypes.h src/*.c > sassafras.c

clean:
	rm -f sassafras sassafras.c

all:
	make -C doc
	make -C src
	make -C xcode
	make sassafras

test:
	cd test; for FILE in $$(basename -a -s .in *.in); do echo $$FILE; ../sassafras $$FILE.in | diff - $$FILE.out; done
