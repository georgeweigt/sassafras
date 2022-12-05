.PHONY: clean

# specify prototypes.h in case the file is missing

sassafras: prototypes.h *.h *.c
	make prototypes.h
	gcc -Wall -O0 -o sassafras *.c -lm

prototypes.h: *.c
	make -s -C ../tools make-prototypes
	../tools/make-prototypes . > prototypes.h

clean:
	rm -f sassafras
