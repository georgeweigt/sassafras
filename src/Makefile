.PHONY: clean checkall sync

CFILES := $(shell ls *.c)

sassafras: defs.h prototypes.h $(CFILES)
	gcc -Wall -O0 -o sassafras -include defs.h -include prototypes.h $(CFILES) -lm

prototypes.h: $(CFILES)
	make -s -C ../tools make-prototypes
	../tools/make-prototypes . > prototypes.h

clean:
	rm -f sassafras prototypes.h

checkall:
	make -s -C ../tools wcheck
	../tools/wcheck defs.h
	for FILE in $(CFILES); do ../tools/wcheck $$FILE; done

# sed removes main.c

XFILES := $(shell ls *.c | sed "s/main\.c//")

sync:
	cat defs.h prototypes.h $(XFILES) > ../xcode/src/sassafras.c
