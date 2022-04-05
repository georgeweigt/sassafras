# To build a new prototypes.h:
#
#       gcc make-prototypes.c
#       ./a.out >prototypes.h

CC = gcc
CFLAGS = -Wall -O0

objects = \
main.o \
read_text_file.o \
data_step.o \
proc_anova.o \
proc_means.o \
proc_print.o \
proc_reg.o \
scan.o \
tdist.o

default : $(objects)
	gcc -o sassafras $(objects) -lm

$(objects) : defs.h prototypes.h

clean :
	rm -f *.o a.out sassafras
