.PHONY: default clean

HELPBOOK = Base.lproj/Sassafras.help/Contents/Resources/English.lproj/index.html

# sed removes ../../main.c

FILES := $(shell ls ../../src/*.c | sed "s/\.\.\/\.\.\/src\/main\.c//")

default:
	rsync -t ../../doc/sassafras.html $(HELPBOOK)
	make -C ../../src prototypes.h
	make sassafras.c

sassafras.c: ../../src/LICENSE ../../src/defs.h ../../src/prototypes.h $(FILES)
	cat ../../src/LICENSE ../../src/defs.h ../../src/prototypes.h $(FILES) > sassafras.c

clean:
	rm -f sassafras.c
