#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <setjmp.h>
#define __USE_ISOC99
#include <math.h>
#ifndef NAN
#define NAN nan("0")
#endif

#define STRBUFLEN 1000
#define ERRBUFLEN 1000

#define MAXVAR 100
#define MAXSTAT 12

struct dataset {
	struct dataset *next;
	char *name;
	int nvar;
	int nobs;
	int max;
	struct spec {
		char *name;
		int max_levels;
		int num_levels;
		char **ltab;
		int w; // number of decimal digits after decimal point
		double *v;
	} spec[MAXVAR];
};

// tokens

#define NAME 1001
#define NUMBER 1002
#define STRING 1003
#define ATAT 1004
#define STARSTAR 1005

// keywords

enum {
	KALPHA = 301,
	KANOVA,
	KBY,
	KCARDS,
	KCLASS,
	KCLM,
	KCLM1,
	KCLM2,
	KDATA,
	KDATALINES,
	KDELIMITER,
	KDLM,
	KFIRSTOBS,
	KINFILE,
	KINPUT,
	KLCLM,
	KLSD,
	KMAX,
	KMAXDEC,
	KMEAN,
	KMEANS,
	KMIN,
	KMODEL,
	KN,
	KNOINT,
	KPRINT,
	KPROC,
	KRANGE,
	KREG,
	KRUN,
	KSTD,
	KSTDDEV,
	KSTDERR,
	KSTDMEAN,
	KSUM,
	KT,
	KTITLE,
	KTITLE1,
	KTITLE2,
	KTITLE3,
	KTTEST,
	KUCLM,
	KVAR,
	KWELCH,
};

extern struct dataset *dataset;
extern char *pgm;
extern char *inp;
extern char *token_str;
extern int token;
extern char strbuf[];
extern char errbuf[];
extern double token_num;
extern FILE *infile;
extern double alpha;
extern int maxdec;
extern int nstats;
extern int stats[];
extern int nvar;
extern int var[];
extern int nby;
extern int by[];
extern int nclass;
extern int class[];
extern char *title;
extern char *title1;
extern char *title2;
extern char *title3;

#define FREE(x) if (x) { free(x); x = NULL; }

#include "prototypes.h"
