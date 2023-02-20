.PHONY: test

FILES := $(shell ls *.in | sed "s/\.in//")

test:
	for FILE in $(FILES); do echo $$FILE; ../src/sassafras $$FILE.in | diff - $$FILE.out; done
