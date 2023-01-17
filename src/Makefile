.PHONY: clean sync

sassafras: defs.h prototypes.h *.c
	gcc -Wall -O0 -o sassafras -include defs.h -include prototypes.h *.c -lm

prototypes.h: *.c
	make -s -C ../tools make-prototypes
	../tools/make-prototypes . > prototypes.h

clean:
	rm -f sassafras prototypes.h

# sed removes main.c

files := $(shell ls *.c | sed "s/main\.c//")

sync:
	cat defs.h prototypes.h $(files) > ../xcode/src/sassafras.c
