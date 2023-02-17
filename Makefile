.PHONY: clean

CFILES := $(shell ls src/*.c)

sassafras: sassafras.c
	gcc -Wall -O0 -o sassafras sassafras.c -lm

sassafras.c: src/defs.h src/prototypes.h $(CFILES)
	cat src/defs.h src/prototypes.h $(CFILES) > sassafras.c

clean:
	rm -f sassafras sassafras.c
