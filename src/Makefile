.PHONY: clean

# To build a new prototypes.h:
#
#       gcc make-prototypes.c
#       ./a.out >prototypes.h

CC = gcc
CFLAGS = -Wall -O0

objects = \
main.o \
read_text_file.o \
globals.o \
data_step.o \
proc_anova.o \
proc_means.o \
proc_print.o \
proc_reg.o \
scan.o \
tdist.o \
parse_by_stmt.o \
parse_class_stmt.o \
parse_comment_stmt.o \
parse_var_stmt.o \
proc_step.o \
print.o \
title_stmt.o \
run.o

sassafras: $(objects)
	gcc -O0 -o sassafras $(objects) -lm

$(objects): defs.h prototypes.h

clean:
	rm -f *.o a.out sassafras
