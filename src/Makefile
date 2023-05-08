.PHONY: clean sync check test

sassafras: defs.h prototypes.h *.c
	$(CC) -Wall -O0 -o sassafras -include defs.h -include prototypes.h *.c -lm

prototypes.h: *.c
	make -s -C ../tools make-prototypes
	../tools/make-prototypes *.c > prototypes.h

clean:
	rm -f sassafras prototypes.h

# sed removes main.c

FILES := $(shell ls *.c | sed "s/main\.c//")

sync:
	make -s prototypes.h
	cat defs.h prototypes.h $(FILES) > ../xcode/src/sassafras.c

check:
	make -s -C ../tools wcheck
	../tools/wcheck *.c

test:
	make -s sassafras
	cd ../test; for FILE in *.in; do echo $$FILE; ../src/sassafras $$FILE | diff - `echo $$FILE | sed "s/\.in/\.out/"`; done
