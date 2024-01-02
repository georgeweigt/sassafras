.PHONY: default clean

# sed removes ../../main.c

FILES := $(shell ls ../../src/*.c | sed "s/\.\.\/\.\.\/src\/main\.c//")

default:
	make -C ../../src prototypes.h
	make sassafras.c

sassafras.c: ../../src/LICENSE ../../src/defs.h ../../src/prototypes.h $(FILES)
	cat ../../src/LICENSE ../../src/defs.h ../../src/prototypes.h $(FILES) > sassafras.c

clean:
	rm -f sassafras.c
