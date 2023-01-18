#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <setjmp.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#define STRBUFLEN 1000
#define ERRBUFLEN 2000

#define MAXVAR 100
#define MAXSTAT 12

#define MAXLVL 100
#define MAXITEM 100

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

extern char *fmt[9];

extern char *pgm;
extern FILE *infile;
extern char *inp;
extern char *title1;
extern char *title2;
extern char *title3;
extern char *title;
extern char *token_str;
extern char buf[10000];
extern char errbuf[ERRBUFLEN];
extern char strbuf[STRBUFLEN];
extern double *B;
extern double *GG;
extern double *PVAL;
extern double *SE;
extern double *TT;
extern double *TVAL;
extern double *XX;
extern double *Y;
extern double *Yhat;
extern double adjrsq;
extern double alpha;
extern double css;
extern double cv;
extern double dfe;
extern double dfm;
extern double dft;
extern double fval;
extern double mean[MAXLVL];
extern double mse;
extern double msr;
extern double pval;
extern double rootmse;
extern double rsquare;
extern double ss[MAXVAR];
extern double sse;
extern double ssr;
extern double sst;
extern double token_num;
extern double variance[MAXLVL];
extern double ybar;
extern int *Z;
extern int *miss;
extern int by[MAXVAR];
extern int class[MAXVAR];
extern int count[MAXLVL];
extern int df[MAXVAR];
extern int gstate;
extern int item[MAXITEM];
extern int kk[MAXVAR];
extern int maxdec;
extern int nby;
extern int nclass;
extern int ncol;
extern int nmiss;
extern int nobs;
extern int noint;
extern int npar;
extern int nrow;
extern int nstats;
extern int num_x;
extern int num_y;
extern int nvar;
extern int nx;
extern int stats[MAXSTAT];
extern int token;
extern int var[MAXVAR];
extern int xtab[MAXVAR];
extern int xx[MAXVAR];
extern int ytab[MAXVAR];
extern int yvar;
extern struct dataset *dataset;
