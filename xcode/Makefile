.PHONY: default clean

HELPBOOK = Base.lproj/Sassafras.help/Contents/Resources/English.lproj/index.html

# sed removes main.c

FILES := $(shell ls ../src/*.c | sed "s/\.\.\/src\/main\.c//")

default:
	rsync -c ../doc/sassafras.html $(HELPBOOK)
	make -C ../src prototypes.h
	make sassafras.c
	xcodebuild -project Sassafras.xcodeproj
	find build -name Sassafras.app -print

sassafras.c: ../src/defs.h ../src/prototypes.h $(FILES)
	cat ../src/defs.h ../src/prototypes.h $(FILES) > sassafras.c

clean:
	rm -f sassafras.c
