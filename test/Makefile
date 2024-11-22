.PHONY: default

default:
	for FILE in $$(basename -a -s .in *.in) ; do echo $$FILE ; ../sassafras $$FILE.in | diff - $$FILE.out ; done
