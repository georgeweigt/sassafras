char *pgm;
FILE *infile;
char *inp;
char *title1;
char *title2;
char *title3;
char *title;
char *token_str;
char buf[10000];
char errbuf[ERRBUFLEN];
char strbuf[STRBUFLEN];
double *B;
double *GG;
double *PVAL;
double *SE;
double *TT;
double *TVAL;
double *XX;
double *Y;
double *Yhat;
double adjrsq;
double alpha;
double css;
double cv;
double dfe;
double dfm;
double dft;
double fval;
double mean[MAXLVL];
double mse;
double msr;
double pval;
double rootmse;
double rsquare;
double ss[MAXVAR];
double sse;
double ssr;
double sst;
double token_num;
double variance[MAXLVL];
double ybar;
int *Z;
int *miss;
int by[MAXVAR];
int class[MAXVAR];
int count[MAXLVL];
int df[MAXVAR];
int gstate;
int item[MAXITEM];
int kk[MAXVAR];
int maxdec;
int nby;
int nclass;
int ncol;
int nmiss;
int nobs;
int noint;
int npar;
int nrow;
int nstats;
int num_x;
int num_y;
int nvar;
int nx;
int stats[MAXSTAT];
int token;
int var[MAXVAR];
int xtab[MAXVAR];
int xx[MAXVAR];
int ytab[MAXVAR];
int yvar;
struct dataset *dataset;

char *fmt[9] = {
	"%0.0f",
	"%0.1f",
	"%0.2f",
	"%0.3f",
	"%0.4f",
	"%0.5f",
	"%0.6f",
	"%0.7f",
	"%0.8f",
};
