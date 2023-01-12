.PHONY: clean

sassafras: sassafras.c
	gcc -Wall -O0 -o sassafras sassafras.c -lm

sassafras.c: src/defs.h src/prototypes.h src/*.c
	cat src/defs.h src/prototypes.h src/*.c > sassafras.c

clean:
	rm -f sassafras sassafras.c
