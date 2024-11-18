.PHONY: default clean test all

default:
	make -C src prototypes.h
	make sassafras

sassafras: sassafras.c
	$(CC) -Wall -O0 -o sassafras sassafras.c -lm

sassafras.c: src/LICENSE src/defs.h src/prototypes.h src/*.c
	cat src/LICENSE src/defs.h src/prototypes.h src/*.c > sassafras.c

clean:
	rm -f sassafras sassafras.c

test:
	make
	cd test; for FILE in *.in; do echo $$FILE; ../sassafras $$FILE | diff - `echo $$FILE | sed "s/\.in/\.out/"`; done

all:
	make -C src
	make -C xcode
	make sassafras
