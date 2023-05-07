.PHONY: clean test

sassafras: sassafras.c
	$(CC) -Wall -O0 -o sassafras sassafras.c -lm

sassafras.c: src/defs.h src/prototypes.h src/*.c
	cat src/defs.h src/prototypes.h src/*.c > sassafras.c

src/prototypes.h: src/*.c
	make -C src prototypes.h

clean:
	rm -f sassafras sassafras.c

test:
	make -s sassafras
	cd test; for FILE in *.in; do echo $$FILE; ../sassafras $$FILE | diff - `echo $$FILE | sed "s/\.in/\.out/"`; done
