.PHONY: all test clean

sassafras: sassafras.c
	$(CC) -Wall -O0 -o sassafras sassafras.c -lm

sassafras.c: src/LICENSE src/defs.h src/prototypes.h src/*.c
	cat src/LICENSE src/defs.h src/prototypes.h src/*.c > sassafras.c

src/prototypes.h: src/*.c
	make -C src prototypes.h

all:
	make sassafras
	make -C doc
	make -C xcode

test:
	cd test; for FILE in $$(basename -a -s .in *.in); do echo $$FILE; ../sassafras $$FILE.in | diff - $$FILE.out; done

clean:
	rm -f sassafras src/sassafras tools/make-prototypes tools/wcheck
