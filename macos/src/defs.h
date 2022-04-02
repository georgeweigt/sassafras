#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#define ZZZ printf("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

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

// global variables

extern struct dataset *dataset;

// settings

extern double alpha;

extern int maxdec;

extern int nstat;
extern int stat[];

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

// functions

extern void var_stmt(void);
extern void keyword(void);
extern char *get_dataline(char *, int);

extern void scan(void);
extern void emit_line(char *);
extern void *xmalloc(int);
extern void *xcalloc(int);
extern void *xrealloc(void *, int);
extern void syntax(void);
extern void stop(char *);

// tokens

#define NAME 1001
#define NUMBER 1002
#define STRING 1003
#define ATAT 1004
#define STARSTAR 1005

extern char *inp;
extern char *token_str;
extern int token;
extern char errbuf[];
extern char strbuf[]; // defined in scan.c
extern double token_num;
extern FILE *infile;

// parsing

extern void data_step(void);
extern void proc_step(void);

extern void parse_default(void);

extern void parse_proc_means(void);
extern void parse_proc_means_body(void);
extern void run_proc_means(void);

extern void parse_proc_print(void);
extern void parse_proc_print_body(void);
extern void run_proc_print(void);

// options

extern void parse_data_option(void);
extern void parse_maxdec_option(void);

// statements

extern void by_stmt(void);
extern void class_stmt(void);
extern void comment_stmt(void);
extern void model_stmt(void);
extern void title_stmt(void);
extern void title1_stmt(void);
extern void title2_stmt(void);
extern void title3_stmt(void);
extern void var_stmt(void);

extern double qt(double, double);
extern void free_datasets(void);
extern void expected(char *);

#include "prototypes.h"
