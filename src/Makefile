.PHONY: clean check

sassafras: defs.h prototypes.h *.c
	$(CC) -Wall -O0 -o sassafras -include defs.h -include prototypes.h *.c -lm

prototypes.h: *.c
	make -s -C ../tools make-prototypes
	../tools/make-prototypes *.c > prototypes.h

clean:
	rm -f sassafras prototypes.h

check:
	make -s -C ../tools wcheck
	../tools/wcheck *.c
