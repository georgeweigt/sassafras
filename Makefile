.PHONY: clean test

CFILES := $(shell ls src/*.c)

sassafras: sassafras.c
	gcc -Wall -O0 -o sassafras sassafras.c -lm

sassafras.c: src/defs.h src/prototypes.h $(CFILES)
	cat src/defs.h src/prototypes.h $(CFILES) > sassafras.c

src/prototypes.h: $(CFILES)
	make -C src prototypes.h

clean:
	rm -f sassafras sassafras.c

TESTFILES := $(shell basename test/*.in | sed "s/\.in//")

test:
	make -s sassafras
	cd test; for FILE in $(TESTFILES); do echo $$FILE; ../sassafras $$FILE.in | diff - $$FILE.out; done
