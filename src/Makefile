.PHONY: clean sync check test

CFILES := $(shell ls *.c)

sassafras: defs.h prototypes.h $(CFILES)
	gcc -Wall -O0 -o sassafras -include defs.h -include prototypes.h $(CFILES) -lm

prototypes.h: $(CFILES)
	make -s -C ../tools make-prototypes
	../tools/make-prototypes . > prototypes.h

clean:
	rm -f sassafras prototypes.h

# sed removes main.c

XFILES := $(shell ls *.c | sed "s/main\.c//")

sync:
	make -s
	cat defs.h prototypes.h $(XFILES) > ../xcode/src/sassafras.c

check:
	make -s
	make -s -C ../tools wcheck
	for FILE in *.c; do ../tools/wcheck $$FILE; done

TESTFILES := $(shell basename ../test/*.in | sed "s/\.in//")

test:
	make -s
	cd ../test; for FILE in $(TESTFILES); do echo $$FILE; ../src/sassafras $$FILE.in | diff - $$FILE.out; done
