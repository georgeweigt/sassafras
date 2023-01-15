#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <setjmp.h>
#include <fcntl.h>
#include <unistd.h>

#define __USE_ISOC99
#include <math.h>
#ifndef NAN
#define NAN nan("0")
#endif

#define STRBUFLEN 1000
#define ERRBUFLEN 1000

#define MAXVAR 100
#define MAXSTAT 12

#define MAXLVL 100
#define MAXITEM 100

#define FREE(x) if (x) { free(x); x = NULL; }

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
extern double *CC;
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
void data_step(void);
void parse_data_body(void);
void read_file(void);
void input_stmt(void);
void parse_data_stmt(void);
void infile_stmt(void);
void datalines_stmt(void);
void get_data(void);
void get_data1(void);
void alloc_data_vectors(void);
void check_data_vectors(void);
void getbuf(void);
void chkbuf(void);
int get_digits(void);
double get_number(void);
void get_string(void);
void dump_data(void);
void free_datasets(void);
void select_dataset(char *s);
void parse_data_expr(void);
void parse_expr(void);
void parse_term(void);
void parse_factor(void);
void parse_log(void);
void emit(int c);
void emit_variable(void);
void emit_number(void);
void catvar(int x, int obs);
double incbeta(double a, double b, double x);
int main(int argc, char **argv);
void emit_line_init(void);
void emit_line(char *s);
void emit_line_center(char *s);
void parse_by_stmt(void);
void parse_class_stmt(void);
void parse_comment_stmt(void);
void parse_var_stmt(void);
void print_table_and_free(char **a, int nrow, int ncol, char *fmt);
void print_table(char **a, int nrow, int ncol, char *fmt);
void print_title(void);
void proc_anova(void);
void proc_anova_parse_stmt(void);
void proc_anova_parse_body(void);
void proc_anova_parse_means_stmt(void);
void proc_anova_parse_means_item(void);
void proc_anova_parse_model_stmt(void);
void proc_anova_parse_model_options(void);
void proc_anova_parse_explanatory_variable(void);
int proc_anova_get_var_index(void);
void proc_anova_add_interaction(int n);
void proc_anova_create_interaction_level_names(int n);
void proc_anova_add_factorial(int n);
void proc_anova_add_nested(int n);
void proc_anova_model(void);
void proc_anova_regression(void);
void proc_anova_prelim(void);
void proc_anova_fit(void);
void proc_anova_fit1(int x, int level);
int proc_anova_compute_G(void);
void proc_anova_compute_B(void);
void proc_anova_compute_Yhat(void);
void proc_anova_compute_ss(void);
void print_anova_table_part1(void);
void print_anova_table_part2(void);
void print_anova_table_part3(void);
void proc_anova_compute_means(int x);
void proc_anova_print_means(int x);
void proc_anova_print_lsd(int x);
void proc_anova_print_ttest(int x);
void proc_means(void);
void parse_proc_means_stmt(void);
void parse_proc_means_body(void);
void run_proc_means(void);
void f(int k);
void g(void);
void h(int varnum);
void proc_print(void);
void parse_proc_print_stmt(void);
void parse_proc_print_body(void);
void run_proc_print(void);
void proc_reg(void);
void proc_reg_parse_stmt(void);
void proc_reg_parse_body(void);
void proc_reg_parse_model_stmt(void);
void proc_reg_parse_model_options(void);
void proc_reg_compute_X(void);
void proc_reg_compute_T(void);
int proc_reg_compute_G(void);
void proc_reg_compute_B(void);
void proc_reg_compute_mse(void);
void proc_reg_compute_C(void);
void proc_reg_compute_SE(void);
void proc_reg_compute_TVAL(void);
void proc_reg_compute_PVAL(void);
void proc_reg_print_B(void);
void proc_reg_print_X(void);
void proc_reg_print_T(void);
void proc_reg_print_G(void);
void proc_reg_print_Z(void);
void proc_reg_regress(void);
void proc_reg_print_parameter_estimates(void);
void proc_reg_print_anova_table(void);
void proc_reg_print_diag_table(void);
void proc_step(void);
char * read_text_file(char *filename);
void run(char *s);
void run_nib(char *s);
void parse_default(void);
void parse_alpha_option(void);
void parse_data_option(void);
void parse_maxdec_option(void);
void * xmalloc(int size);
void * xrealloc(void *p, int size);
void print_pgm(void);
void stop(char *s);
void expected(char *s);
void scan(void);
int scan1(void);
void keyword(void);
char * get_dataline(char *buf, int len);
void get_next_token(void);
double tdist(double t, double df);
double qt(double p, double df);
double fdist(double t, double df1, double df2);
void title_stmt(void);
void title1_stmt(void);
void title2_stmt(void);
void title3_stmt(void);
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
double *CC;
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
char lbuf[100]; // level name buffer

int nbytecode;
unsigned char bytecode[10000];

#define TOS 100
double stack[TOS];
int tos;

// dd is the head of the linked list of datsets

struct dataset *dd;

char *inb;
char filename[1000];
char delim[100];
int ctrl;
int firstobs;

enum {
	SCAN_NUM,
	SCAN_STR,
	NEG,
	ADD,
	SUB,
	MUL,
	DIV,
	POW,
	LOG,
	LIT,
	LOAD,
	STORE,
	GETBUF,
	CHKBUF,
};

// The DATALINES statement is the last statement in the DATA step and
// immediately precedes the first data line.

void
data_step(void)
{
	ctrl = 0;
	firstobs = 1;
	nbytecode = 0;
	*filename = 0;

	strcpy(delim, "\t ,");

	parse_data_stmt();

	parse_data_body();
}

// parse statements that follow DATA statement

void
parse_data_body(void)
{
	for (;;) {

		if (token == NAME) {

			// look ahead for NAME = EXPR

			while (*inp == ' ' || *inp == '\t')
				inp++;

			if (*inp == '=') {
				parse_data_expr();
				continue;
			}
		}

		keyword();

		switch (token) {
		case 0:
		case KDATA:
		case KPROC:
		case KRUN:
			if (*filename == 0)
				expected("infile or datalines");
			read_file();
			return;
		case KDATALINES:
			if (*filename)
				stop("Datalines after infile?");
			datalines_stmt();
			return;
		case KINPUT:
			input_stmt();
			break;
		case KINFILE:
			infile_stmt();
			break;
		default:
			parse_default();
			break;
		}
	}
}

void
read_file(void)
{
	infile = fopen(filename, "r");

	if (infile == NULL) {
		snprintf(errbuf, ERRBUFLEN, "Cannot open %s", filename);
		stop(errbuf);
	}

	get_data();

	fclose(infile);
	infile = NULL;
}

void
input_stmt(void)
{
	int n;

	scan(); // get next token after INPUT

	// scan data specifiers

	for (;;) {

		if (token != NAME)
			break;

		if (dd->nvar == MAXVAR)
			stop("Too many variables");

		n = dd->nvar++;

		dd->spec[n].name = strdup(strbuf);

		scan();

		if (token == '$') {
			dd->spec[n].max_levels = 10;
			dd->spec[n].num_levels = 0;
			dd->spec[n].ltab = (char **) xmalloc(10 * sizeof (char *));
			emit(SCAN_STR);
			emit(n);
			scan();
		} else {
			dd->spec[n].max_levels = 0;
			dd->spec[n].num_levels = 0;
			dd->spec[n].ltab = NULL;
			emit(SCAN_NUM);
			emit(n);
		}
	}

	if (token == ATAT) {
		emit(CHKBUF);
		scan();
	} else
		emit(GETBUF);

	if (token != ';')
		expected("end of statement");

	scan();
}

void
parse_data_stmt(void)
{
	struct dataset *d;

	d = (struct dataset *) xmalloc(sizeof (struct dataset));

	bzero(d, sizeof (struct dataset));

	d->next = dd;
	dd = d;

	d->max = 1000;

	scan();

	if (token == NAME) {
		d->name = strdup(strbuf); // dataset NAME
		scan();
	}

	if (token != ';')
		expected("end of statment");

	scan();
}

void
infile_stmt(void)
{
	if (filename[0])
		stop("Multiple INFILE statements");

	get_next_token();

	switch (token) {
	case STRING:
		strcpy(filename, strbuf);
		break;
	case KCARDS:
	case KDATALINES:
		break;
	default:
		expected("file name");
	}

	for (;;) {

		scan();
		keyword();

		switch (token) {

		case ';':
			scan();
			return;

		case KDLM:
		case KDELIMITER:
			scan();
			if (token != '=')
				expected("equals sign");
			scan();
			if (token != STRING)
				expected("string");
			if (strlen(strbuf) >= sizeof delim)
				stop("string too long");
			strcpy(delim, strbuf);
			break;

		case KFIRSTOBS:
			scan();
			if (token != '=')
				expected("equals sign");
			scan();
			if (token != NUMBER)
				expected("number");
			if (sscanf(strbuf, "%d", &firstobs) != 1)
				stop("Error in number syntax");
			break;

		default:
			expected("end of statement");
		}
	}
}

void
datalines_stmt(void)
{
	// don't allow garbage after datalines token

	while (isspace(*inp) && *inp != '\n' && *inp != '\r')
		inp++;

	if (*inp == ';') {
		inp++;
		while (isspace(*inp) && *inp != '\n' && *inp != '\r')
			inp++;
	}

	if (*inp != '\n' && *inp != '\r') {
		scan();
		expected("datalines data");
	}

	get_data();

	scan(); // get next token (current token is still DATALINES)
}

void
get_data(void)
{
	alloc_data_vectors();
	getbuf();
	while (inb) {
		check_data_vectors();
		get_data1();
		dd->nobs++;
	}
}

// get one row of observations

void
get_data1(void)
{
	int i, j, n, w, x;

	double t;

	// observation number

	n = dd->nobs;

	// process bytecodes

	tos = 0;

	for (i = 0; i < nbytecode; i++) {
		switch (bytecode[i]) {

		case SCAN_NUM:
			x = bytecode[++i];
			w = get_digits();
			t = get_number();
			if (w > dd->spec[x].w)
				dd->spec[x].w = w;
			dd->spec[x].v[n] = t;
			break;

		case SCAN_STR:
			x = bytecode[++i];
			get_string();
			catvar(x, n);
			break;

		case LOAD:
			if (tos == TOS)
				stop("Stack overflow");
			x = bytecode[++i];
			stack[tos++] = dd->spec[x].v[n];
			break;

		case STORE:
			if (tos == 0)
				stop("Stack underrun");
			x = bytecode[++i];
			t = stack[--tos];
			dd->spec[x].w = 4; // default precision
			dd->spec[x].v[n] = t;
			break;

		case LIT:
			if (tos == TOS)
				stop("Stack overflow");
			for (j = 0; j < sizeof (double); j++)
				((char *) &t)[j] = bytecode[++i];
			stack[tos++] = t;
			break;

		case NEG:
			if (tos == 0)
				stop("Stack underrun");
			stack[tos - 1] = -stack[tos - 1];
			break;

		case ADD:
			if (tos < 2)
				stop("Stack underrun");
			t = stack[--tos];
			stack[tos - 1] += t;
			break;

		case SUB:
			if (tos < 2)
				stop("Stack underrun");
			t = stack[--tos];
			stack[tos - 1] -= t;
			break;

		case MUL:
			if (tos < 2)
				stop("Stack underrun");
			t = stack[--tos];
			stack[tos - 1] *= t;
			break;

		case DIV:
			if (tos < 2)
				stop("Stack underrun");
			t = stack[--tos];
			stack[tos - 1] /= t;
			break;

		case POW:
			if (tos < 2)
				stop("Stack underrun");
			t = stack[--tos];
			stack[tos - 1] = pow(stack[tos - 1], t);
			break;

		case LOG:
			if (tos < 1)
				stop("Stack underrun");
			stack[tos - 1] = log(stack[tos - 1]);
			break;

		case GETBUF:
			getbuf();
			break;

		case CHKBUF:
			chkbuf();
			break;

		default:
			stop("Bytecode error");
		}
	}
}

void
alloc_data_vectors(void)
{
	int i;
	for (i = 0; i < dd->nvar; i++)
		if (dd->spec[i].v == NULL)
			dd->spec[i].v = (double *) xmalloc(dd->max * sizeof (double));
}

// increase length of data vectors if necessary

void
check_data_vectors(void)
{
	int i;
	if (dd->nobs < dd->max)
		return;
	dd->max += 1000;
	for (i = 0; i < dd->nvar; i++)
		dd->spec[i].v = xrealloc(dd->spec[i].v, dd->max * sizeof (double));
}

void
getbuf(void)
{
	do {
		if (infile)
			for (;;) {
				inb = fgets(buf, sizeof buf, infile);
				if (inb == NULL || firstobs < 2)
					break;
				firstobs--;
			}
		else
			inb = get_dataline(buf, sizeof buf);

		if (inb == NULL)
			return;

		// skip spaces

		while (isspace(*inb))
			inb++;

	} while (*inb == 0); // skip blank lines
}

void
chkbuf(void)
{
	// check for end of input

	if (inb == NULL)
		return;

	// skip spaces

	while (isspace(*inb))
		inb++;

	if (*inb == 0)
		getbuf();
}

// get number of decimal digits

int
get_digits(void)
{
	int n = 0;
	char *s = inb;

	// scan to decimal point

	while (strchr(delim, *s) == NULL && *s != '.')
		s++;

	if (*s != '.')
		return 0;

	while (isdigit(*++s) && n < 8)
		n++;

	return n;
}

// get a number from the input buffer

double
get_number(void)
{
	double d = NAN;

	// check end of input

	if (inb == NULL)
		return d;

	// scan leading spaces

	while (isspace(*inb))
		inb++;

	if (*inb == 0)
		return d;

	if (strchr(delim, *inb)) {
		inb++;
		return d;
	}

	// scan number

	sscanf(inb, "%lg", &d);

	// scan delimiter

	while (strchr(delim, *inb) == NULL)
		inb++;

	if (*inb)
		inb++;

	return d;
}

// get a string from the input buffer

void
get_string(void)
{
	int i, n;
	char *s, *t;

	lbuf[0] = 0;

	// check end of input

	if (inb == NULL)
		return;

	// scan leading spaces

	while (isspace(*inb))
		inb++;

	if (*inb == 0)
		return;

	if (strchr(delim, *inb)) {
		inb++;
		return;
	}

	// start of string

	s = inb;

	// find end of string

	while (strchr(delim, *inb) == NULL)
		inb++;

	// skip trailing spaces

	t = inb;
	while (isspace(t[-1]))
		t--;

	n = (int) (t - s);

	if (n + 1 > sizeof lbuf)
		stop("In the observed data, a level name is too long");

	// missing data char?

	if (n == 1 && *s == '.')
		return;

	// copy the string into lbuf

	for (i = 0; i < n; i++)
		lbuf[i] = s[i];

	lbuf[n] = 0;

	// scan delimiter

	if (*inb)
		inb++;
}

void
dump_data(void)
{
	int i, j;
	double d;

	printf("dump_data: %s nvar=%d\n", dd->name, dd->nvar);

	for (i = 0; i < dd->nvar; i++)
		printf(" %s", dd->spec[i].name);

	printf("\n");

	for (i = 0; i < dd->nobs; i++) {
		for (j = 0; j < dd->nvar; j++) {
			d = dd->spec[j].v[i];
			if (dd->spec[j].ltab)
				printf(" %s", dd->spec[j].ltab[(int) d]);
			else
				printf(" %g", d);
		}
		printf("\n");
	}
}

void
free_datasets(void)
{
	int i;
	struct dataset *t;

	while (dd) {
		t = dd;
		dd = dd->next;
		if (t->name)
			free(t->name);
		for (i = 0; i < MAXVAR; i++) {
			if (t->spec[i].name)
				free(t->spec[i].name);
			if (t->spec[i].v)
				free(t->spec[i].v);
			if (t->spec[i].ltab)
				free(t->spec[i].ltab);
		}
		free(t);
	}
}

void
select_dataset(char *s)
{
	dataset = dd;
	if (s == NULL)
		return;
	while (dataset) {
		if (dataset->name && strcmp(dataset->name, s) == 0)
			return;
		dataset = dataset->next;
	}
	snprintf(errbuf, ERRBUFLEN, "Dataset %s?", s);
	stop(errbuf);
}

// NAME = EXPR

void
parse_data_expr(void)
{
	if (dd->nvar == MAXVAR)
		stop("Too many variables");
	dd->spec[dd->nvar].name = strdup(strbuf);
	scan(); // skip over name
	scan(); // skip over equals sign
	parse_expr();
	if (token != ';')
		stop("';' expected");
	scan();
	emit(STORE);
	emit(dd->nvar);
	dd->nvar++;
}

void
parse_expr(void)
{
	switch (token) {
	case '+':
		scan();
		parse_term();
		break;
	case '-':
		scan();
		parse_term();
		emit(NEG);
		break;
	default:
		parse_term();
		break;
	}

	for (;;) {
		switch (token) {
		case '+':
			scan();
			parse_term();
			emit(ADD);
			break;
		case '-':
			scan();
			parse_term();
			emit(SUB);
			break;
		default:
			return;
		}
	}
}

void
parse_term(void)
{
	parse_factor();

	for (;;) {
		switch (token) {
		case '*':
			scan();
			parse_factor();
			emit(MUL);
			break;
		case '/':
			scan();
			parse_factor();
			emit(DIV);
			break;
		default:
			return;
		}
	}
}

void
parse_factor(void)
{
	switch (token) {
	case NAME:
		if (strcmp(strbuf, "LOG") == 0)
			parse_log();
		else {
			emit_variable();
			scan();
		}
		break;
	case NUMBER:
		emit_number();
		scan();
		break;
	case '(':
		scan();
		parse_expr();
		if (token != ')')
			stop("Syntax error");
		scan();
		break;
	default:
		stop("Syntax error");
	}

	if (token == STARSTAR) {
		scan();
		switch (token) {
		case '+':
			scan();
			parse_factor();
			break;
		case '-':
			scan();
			parse_factor();
			emit(NEG);
			break;
		default:
			parse_factor();
			break;
		}
		emit(POW);
	}
}

void
parse_log(void)
{
	// look ahead

	while (isspace(*inp) && *inp != '\n' && *inp != '\r')
		inp++;

	if (*inp != '(') {
		emit_variable();
		scan();
		return;
	}

	scan();
	scan();

	parse_expr();

	if (token != ')')
		expected(")");

	scan();

	emit(LOG);
}

void
emit(int c)
{
	if (nbytecode == sizeof bytecode)
		stop("Bytecode buffer overrun");
	bytecode[nbytecode++] = c;
}

// Variable name is in strbuf

void
emit_variable(void)
{
	int i;
	for (i = 0; i < dd->nvar; i++)
		if (strcmp(dd->spec[i].name, strbuf) == 0)
			break;
	if (i == dd->nvar)
		stop("Variable not found");
	if (dd->spec[i].ltab)
		stop("An arithemetic expression cannot use a category variable");
	emit(LOAD);
	emit(i);
}

void
emit_number(void)
{
	int i;
	char *p = (char *) &token_num;
	emit(LIT);
	for (i = 0; i < sizeof (double); i++)
		emit(p[i]);
}

// To here with category level from input data stream
//
//	x	Variable index
//
//	obs	Observation number

void
catvar(int x, int obs)
{
	int i, m, n;

	// missing data?

	if (lbuf[0] == 0) {
		dd->spec[x].v[obs] = NAN;
		return;
	}

	m = dd->spec[x].max_levels;
	n = dd->spec[x].num_levels;

	// search for the level name

	for (i = 0; i < n; i++)
		if (strcmp(dd->spec[x].ltab[i], lbuf) == 0)
			break;

	// check for new level name

	if (i == n) {

		// get space for more levels if necessary

		if (m == n) {
			m += 100;
			dd->spec[x].max_levels = m;
			dd->spec[x].ltab = xrealloc(dd->spec[x].ltab, m * sizeof (char *));
		}

		dd->spec[x].ltab[i] = strdup(lbuf);
		dd->spec[x].num_levels++;
	}

	// save the level number

	dd->spec[x].v[obs] = i;
}
/*
 * zlib License
 *
 * Regularized Incomplete Beta Function
 *
 * Copyright (c) 2016, 2017 Lewis Van Winkle
 * http://CodePlea.com
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgement in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */


#define STOP 1.0e-8
#define TINY 1.0e-30

double
incbeta(double a, double b, double x)
{
	if (x < 0.0 || x > 1.0) return 1.0/0.0;

	/*The continued fraction converges nicely for x < (a+1)/(a+b+2)*/
	if (x > (a+1.0)/(a+b+2.0)) {
		return (1.0-incbeta(b,a,1.0-x)); /*Use the fact that beta is symmetrical.*/
	}

	/*Find the first part before the continued fraction.*/
	const double lbeta_ab = lgamma(a)+lgamma(b)-lgamma(a+b);
	const double front = exp(log(x)*a+log(1.0-x)*b-lbeta_ab) / a;

	/*Use Lentz's algorithm to evaluate the continued fraction.*/
	double f = 1.0, c = 1.0, d = 0.0;

	int i, m;
	for (i = 0; i <= 200; ++i) {
		m = i/2;

		double numerator;
		if (i == 0) {
			numerator = 1.0; /*First numerator is 1.0.*/
		} else if (i % 2 == 0) {
			numerator = (m*(b-m)*x)/((a+2.0*m-1.0)*(a+2.0*m)); /*Even term.*/
		} else {
			numerator = -((a+m)*(a+b+m)*x)/((a+2.0*m)*(a+2.0*m+1)); /*Odd term.*/
		}

		/*Do an iteration of Lentz's algorithm.*/
		d = 1.0 + numerator * d;
		if (fabs(d) < TINY) d = TINY;
		d = 1.0 / d;

		c = 1.0 + numerator / c;
		if (fabs(c) < TINY) c = TINY;

		const double cd = c*d;
		f *= cd;

		/*Check for stop.*/
		if (fabs(1.0-cd) < STOP) {
			return front * (f-1.0);
		}
	}

	return 1.0/0.0; /*Needed more loops, did not converge.*/
}

int
main(int argc, char **argv)
{
	char *s;

	if (argc < 2) {
		fprintf(stderr, "usage: sassafras filename\n");
		return 0;
	}

	s = read_text_file(argv[1]);

	run(s);

	free(s);
}

void
emit_line_init(void)
{
}

void
emit_line(char *s)
{
	printf("%s\n", s);
}

void
emit_line_center(char *s)
{
	int i, n;

	n = strlen(s);

	n = (80 - n) / 2;

	for (i = 0; i < n; i++)
		printf(" ");

	printf("%s\n", s);
}
void
parse_by_stmt(void)
{
	int i;

	for (;;) {

		scan();

		if (token == ';')
			break;

		if (token != NAME)
			stop("Variable name expected");

		// look for match in dataset

		for (i = 0; i < dataset->nvar; i++)
			if (strcmp(dataset->spec[i].name, strbuf) == 0)
				break;

		if (i == dataset->nvar) {
			snprintf(errbuf, ERRBUFLEN, "The variable %s not in the dataset", strbuf);
			stop(errbuf);
		}

		if (dataset->spec[i].ltab == NULL) {
			snprintf(errbuf, ERRBUFLEN, "The variable %s is not a categorical variable", strbuf);
			stop(errbuf);
		}

		by[nby++] = i;
	}

	scan(); // eat the semicolon
}
void
parse_class_stmt(void)
{
	int i;

	for (;;) {

		scan();

		if (token == ';')
			break;

		if (token != NAME)
			stop("Variable name expected");

		// look for match in dataset

		for (i = 0; i < dataset->nvar; i++)
			if (strcmp(dataset->spec[i].name, strbuf) == 0)
				break;

		if (i == dataset->nvar) {
			snprintf(errbuf, ERRBUFLEN, "The variable %s not in the dataset", strbuf);
			stop(errbuf);
		}

		if (dataset->spec[i].ltab == NULL) {
			snprintf(errbuf, ERRBUFLEN, "The variable %s is not a categorical variable", strbuf);
			stop(errbuf);
		}

		class[nclass++] = i;
	}

	scan(); // eat the semicolon
}
void
parse_comment_stmt(void)
{
	while (*inp && *inp != ';')
		inp++;
	scan();
}
void
parse_var_stmt(void)
{
	int i;

	for (;;) {

		scan();

		if (token == ';')
			break;

		if (token != NAME)
			stop("Variable name expected");

		// look for match in dataset

		for (i = 0; i < dataset->nvar; i++)
			if (strcmp(dataset->spec[i].name, strbuf) == 0)
				break;

		if (i == dataset->nvar) {
			snprintf(errbuf, ERRBUFLEN, "Variable %s?", strbuf);
			stop(errbuf);
		}

		var[nvar++] = i;
	}

	scan(); // eat the semicolon
}
#define A(i, j) (a + (i) * ncol)[j]

void
print_table_and_free(char **a, int nrow, int ncol, char *fmt)
{
	int i, j;
	print_table(a, nrow, ncol, fmt);
	for (i = 0; i < nrow; i++)
		for (j = 0; j < ncol; j++)
			free(A(i, j));
	free(a);
}

void
print_table(char **a, int nrow, int ncol, char *fmt)
{
	int c, i, j, k, n, nsp = 0, t, *w;
	char *b, *buf, *s;

	w = (int *) xmalloc(ncol * sizeof (int));

	// measure column widths

	t = 0;

	for (j = 0; j < ncol; j++) {
		w[j] = 0;
		for (i = 0; i < nrow; i++) {
			s = A(i, j);
			if (s == NULL)
				continue;
			n = (int) strlen(s);
			if (n > w[j])
				w[j] = n;
		}
		t += w[j];
	}

	// spaces between columns

	if (ncol > 1) {
		nsp = (80 - t) / (ncol - 1);
		if (nsp < 2)
			nsp = 2;
		if (nsp > 5)
			nsp = 5;
		t += (ncol - 1) * nsp;
	}

	// number of spaces to center the line

	if (t < 80)
		c = (80 - t) / 2;
	else
		c = 0;

	// alloc line buffer

	buf = (char *) xmalloc(c + t + 1);

	// leading spaces

	memset(buf, ' ', c);

	// for each row

	for (i = 0; i < nrow; i++) {

		b = buf + c;

		// for each column

		for (j = 0; j < ncol; j++) {

			// space between columns

			if (j > 0) {
				memset(b, ' ', nsp);
				b += nsp;
			}

			s = A(i, j);

			if (s == NULL) {
				memset(b, ' ', w[j]);
				b += w[j];
			} else {

				n = (int) strlen(s);

				k = w[j] - n;

				switch (fmt[j]) {

				// right aligned

				case 0:
					memset(b, ' ', k);
					b += k;
					strcpy(b, s);
					b += n;
					break;

				// left aligned

				case 1:
					strcpy(b, s);
					b += n;
					memset(b, ' ', k);
					b += k;
					break;
				}
			}
		}

		*b = 0;

		emit_line(buf);
	}

	free(buf);
	free(w);

	emit_line("");
}

void
print_title(void)
{
	if (title)
		emit_line_center(title);

	if (title1)
		emit_line_center(title1);

	if (title2)
		emit_line_center(title2);

	if (title3)
		emit_line_center(title3);

	if (title || title1 || title2 || title3)
		emit_line("");
}

#undef A
//	B	Regression coefficients
//
//	css	Corrected sum of squares
//
//	df	Degrees of freedom
//
//	dfe	Degrees of freedom error
//
//	G	Inverse of X'X
//
//	gstate	Current state of G
//
//	miss	Missing vector
//
//	ncol	Number of columns in design matrix
//
//	nobs	Number of rows in design matrix
//
//	npar	Number of fit parameters
//
//	nx	Number of explanatory variables
//
//	ss	Sequential sum of squares regression
//
//	ssr	Sum of squares regression
//
//	sse	Sum of squares error
//
//	T	Temporary matrix
//
//	X	Design matrix
//
//	xtab	List of explanatory variables
//
//	Y	Response vector
//
//	Yhat	Predicted response X * B
//
//	ybar	Mean of response variable Y

#define G(i, j) (GG + (i) * ncol)[j]
#define T(i, j) (TT + (i) * ncol)[j]
#define X(i, j) (XX + (i) * ncol)[j]

void
proc_anova(void)
{
	nx = 0;

	proc_anova_parse_stmt();

	if (dataset == NULL)
		stop("No data set");

	proc_anova_parse_body();
}

void
proc_anova_parse_stmt(void)
{
	for (;;) {
		scan();
		keyword();
		switch (token) {
		case ';':
			scan(); // eat the semicolon
			return;
		case KDATA:
			parse_data_option();
			break;
		default:
			expected("end of statement or data=name");
		}
	}
}

void
proc_anova_parse_body(void)
{
	for (;;) {
		keyword();
		switch (token) {
		case 0:
		case KDATA:
		case KPROC:
		case KRUN:
			return;
		case KCLASS:
			parse_class_stmt();
			break;
		case KMEANS:
			proc_anova_parse_means_stmt();
			break;
		case KMODEL:
			proc_anova_parse_model_stmt();
			proc_anova_model();
			break;
		default:
			parse_default();
			break;
		}
	}
}

void
proc_anova_parse_means_stmt(void)
{
	int i, lsd, n, ttest, x;
	char *s;

	alpha = 0.05;
	ttest = 0;
	lsd = 0;
	n = 0;

	scan();

	for (;;) {

		if (token == ';' || token == '/')
			break;

		proc_anova_parse_means_item();

		for (i = 0; i < dataset->nvar; i++) {
			s = dataset->spec[i].name;
			if (strcmp(s, buf) == 0)
				break;
		}

		if (i == dataset->nvar)
			stop("Not in data set");

		if (dataset->spec[i].ltab == NULL)
			stop("Not categorical");

		if (n == MAXITEM)
			stop("Buffer overrun");

		item[n++] = i;
	}

	// parse options

	if (token == '/') {
		scan();
		while (token != ';') {
			keyword();
			switch (token) {
			case KALPHA:
				parse_alpha_option();
				break;
			case KT:
			case KLSD:
				lsd = 1;
				break;
			case KTTEST:
				ttest = 1;
				break;
			default:
				expected("statement option");
				break;
			}
			scan();
		}
	}

	scan(); // eat end of line

	for (i = 0; i < n; i++) {
		x = item[i];
		proc_anova_compute_means(x);
		proc_anova_print_means(x);
		if (lsd)
			proc_anova_print_lsd(x);
		if (ttest)
			proc_anova_print_ttest(x);
	}
}

// An item is a variable name or an interaction

// Returns token in buf[]

void
proc_anova_parse_means_item(void)
{
	if (token != NAME)
		expected("variable name");

	if (strlen(strbuf) + 1 > sizeof buf)
		stop("Buffer overrun");

	strcpy(buf, strbuf);

	scan();

	while (token == '*') {
		scan();
		if (token != NAME)
			expected("variable name");
		if (strlen(buf) + strlen(strbuf) + 2 > sizeof buf)
			stop("Buffer overrun");
		strcat(buf, "*");
		strcat(buf, strbuf);
		scan();
	}
}

void
proc_anova_parse_model_stmt(void)
{
	int i;

	nx = 0;

	// parse dependent variable

	scan();

	if (token != NAME)
		expected("variable name");

	for (i = 0; i < dataset->nvar; i++)
		if (strcmp(dataset->spec[i].name, strbuf) == 0)
			break;

	if (i == dataset->nvar)
		expected("variable in data set");

	if (dataset->spec[i].ltab)
		expected("numeric variable");

	yvar = i;

	scan();

	if (token != '=')
		expected("equals sign");

	scan();

	// parse explanatory variables

	for (;;) {

		if (token == ';' || token == '/')
			break;

		if (token != NAME)
			expected("variable name");

		proc_anova_parse_explanatory_variable();
	}

	if (token == '/')
		proc_anova_parse_model_options();

	scan(); // eat the semicolon
}

void
proc_anova_parse_model_options(void)
{
	for (;;) {

		scan();

		keyword();

		switch (token) {

		case ';':
			return;

		default:
			stop("';' or MODEL option expected");
		}
	}
}

// Explanatory variable forms
//
// 1.	name
//
// 2.	name * name ...
//
// 3.	name | name ...
//
// 4.	name(name)
//
// 5.	name(name name ...)

void
proc_anova_parse_explanatory_variable(void)
{
	int n = 1;

	xx[0] = proc_anova_get_var_index();

	scan();

	switch (token) {

	case '*': // name * name ...

		do {
			scan();
			if (token != NAME)
				expected("variable name");
			if (n == MAXVAR)
				stop("Too many interaction terms");
			xx[n++] = proc_anova_get_var_index();
			scan();
		} while (token == '*');
		proc_anova_add_interaction(n);
		break;

	case '|': // name | name ...

		do {
			scan();
			if (token != NAME)
				expected("variable name");
			if (n == MAXVAR)
				stop("Too many interaction terms");
			xx[n++] = proc_anova_get_var_index();
			scan();
		} while (token == '|');
		proc_anova_add_factorial(n);
		break;

	case '(': // name(name name ...)

		scan();
		while (token == NAME) {
			if (n == MAXVAR)
				stop("Too many interaction terms");
			xx[n++] = proc_anova_get_var_index();
			scan();
		};
		if (token != ')')
			expected("right parenthesis ')'");
		scan();
		proc_anova_add_nested(n);
		break;

	default:
		if (nx == MAXVAR)
			stop("Too many explanatory variables");
		xtab[nx++] = xx[0];
		break;
	}
}

int
proc_anova_get_var_index(void)
{
	int i, n;
	n = dataset->nvar;
	for (i = 0; i < n; i++)
		if (strcmp(dataset->spec[i].name, strbuf) == 0)
			break;
	if (i == n) {
		snprintf(errbuf, ERRBUFLEN, "The variable name \"%s\" is not in the data set", strbuf);
		stop(errbuf);
	}
	if (dataset->spec[i].ltab == NULL) {
		snprintf(errbuf, ERRBUFLEN, "The variable \"%s\" is numeric, please change to categorical in the data step", strbuf);
		stop(errbuf);
	}
	return i;
}

// There are n interaction names in xx[]

void
proc_anova_add_interaction(int n)
{
	int i, j, k, len, m, x;
	char *name, *s;
	double d;

	len = n; // accommodate n - 1 asterisks and 1 end of line

	for (i = 0; i < n; i++) {
		x = xx[i];
		s = dataset->spec[x].name;
		len += strlen(s);
	}

	name = (char *) xmalloc(len);

	name[0] = 0;

	for (i = 0; i < n; i++) {
		if (i > 0)
			strcat(name, "*");
		x = xx[i];
		s = dataset->spec[x].name;
		strcat(name, s);
	}

	// is it already in the data set?

	for (m = 0; m < dataset->nvar; m++)
		if (strcmp(name, dataset->spec[m].name) == 0)
			break;

	if (m < dataset->nvar)
		free(name);
	else {
		if (dataset->nvar == MAXVAR)
			stop("Too many variable names");

		m = dataset->nvar++;

		dataset->spec[m].name = name;

		// number of levels

		k = 1;
		for (i = 0; i < n; i++) {
			x = xx[i];
			k *= dataset->spec[x].num_levels;
		}

		dataset->spec[m].ltab = (char **) xmalloc(k * sizeof (char *));
		dataset->spec[m].max_levels = k;
		dataset->spec[m].num_levels = k;
		dataset->spec[m].v = (double *) xmalloc(dataset->max * sizeof (double));

		proc_anova_create_interaction_level_names(n);

		// data values

		for (i = 0; i < dataset->nobs; i++) {
			d = 0;
			for (j = 0; j < n; j++) {
				x = xx[j];
				d = d * dataset->spec[x].num_levels + dataset->spec[x].v[i];
			}
			dataset->spec[m].v[i] = d;
		}
	}

	// is it already an explanatory variable?

	for (i = 0; i < nx; i++)
		if (xtab[i] == m)
			return;

	if (nx == MAXVAR)
		stop("Too many explanatory variables");

	xtab[nx++] = m;
}

// Create interaction level names for product of n terms

void
proc_anova_create_interaction_level_names(int n)
{
	int i, j, k, len, m, x;
	char *name, *s;

	m = dataset->nvar - 1; // index of new interaction column

	for (i = 0; i < n; i++)
		kk[i] = 0;

	for (i = 0; i < dataset->spec[m].num_levels; i++) {

		len = n;

		for (j = 0; j < n; j++) {
			x = xx[j];
			k = kk[j];
			s = dataset->spec[x].ltab[k];
			len += strlen(s);
		}

		name = (char *) xmalloc(len);

		name[0] = 0;

		for (j = 0; j < n; j++) {
			if (j > 0)
				strcat(name, "*");
			x = xx[j];
			k = kk[j];
			s = dataset->spec[x].ltab[k];
			strcat(name, s);
		}

		dataset->spec[m].ltab[i] = name;

		// increment indices

		for (j = n - 1; j >= 0; j--) {
			x = xx[j];
			if (++kk[j] < dataset->spec[x].num_levels)
				break;
			kk[j] = 0;
		}
	}
}

void
proc_anova_add_factorial(int n)
{
}

void
proc_anova_add_nested(int n)
{
}

void
proc_anova_model(void)
{
	proc_anova_regression();

	print_title();

	if (nmiss) {
		snprintf(buf, sizeof buf, "%d observations deleted due to missing data", nmiss);
		emit_line(buf);
	}

	emit_line_center("Analysis of Variance");
	emit_line("");

	print_anova_table_part1();

	print_anova_table_part2();

	print_anova_table_part3();
}

void
proc_anova_regression(void)
{
	proc_anova_prelim();

	proc_anova_fit();

	dfe = nobs - npar;

	mse = sse / dfe;

	rootmse = sqrt(mse);

	msr = ssr / (npar - 1);

	fval = msr / mse;

	pval = 1 - fdist(fval, npar - 1, nobs - npar);

	rsquare = 1 - sse / css;

	adjrsq = 1 - (double) (nobs - 1) / (nobs - npar) * sse / css;

	cv = 100 * rootmse / ybar;
}

void
proc_anova_prelim(void)
{
	int i, j, k, x;

	FREE(miss)
	FREE(B)
	FREE(GG)
	FREE(TT)
	FREE(XX)
	FREE(Y)
	FREE(Yhat)

	// missing data

	miss = (int *) xmalloc(dataset->nobs * sizeof (int));

	nmiss = 0;

	for (i = 0; i < dataset->nobs; i++) {
		miss[i] = 0;
		if (isnan(dataset->spec[yvar].v[i])) {
			miss[i] = 1;
			nmiss++;
			continue;
		}
		for (j = 0; j < nx; j++) {
			x = xtab[j];
			if (isnan(dataset->spec[x].v[i])) {
				miss[i] = 1;
				nmiss++;
				break;
			}
		}
	}

	nobs = dataset->nobs - nmiss;

	// add up all levels

	ncol = 1; // for intercept

	for (i = 0; i < nx; i++) {
		x = xtab[i];
		ncol += dataset->spec[x].num_levels;
	}

	B = (double *) xmalloc(ncol * sizeof (double));
	GG = (double *) xmalloc(ncol * ncol * sizeof (double));
	TT = (double *) xmalloc(ncol * ncol * sizeof (double));
	XX = (double *) xmalloc(nobs * ncol * sizeof (double));
	Y = (double *) xmalloc(nobs * sizeof (double));
	Yhat = (double *) xmalloc(nobs * sizeof (double));

	// fill Y with non-missing values

	k = 0;

	for (i = 0; i < dataset->nobs; i++) {
		if (miss[i])
			continue;
		Y[k++] = dataset->spec[yvar].v[i];
	}

	ybar = 0;

	for (i = 0; i < nobs; i++)
		ybar += Y[i];

	ybar /= nobs;

	// corrected sum of squares

	css = 0;

	for (i = 0; i < nobs; i++)
		css += (Y[i] - ybar) * (Y[i] - ybar);
}

void
proc_anova_fit(void)
{
	int i, j, n, x;

	// put in intercept

	for (i = 0; i < nobs; i++)
		X(i, 0) = 1;

	npar = 1;

	// fit explanatory variables

	for (i = 0; i < nx; i++) {
		x = xtab[i];
		n = dataset->spec[x].num_levels;
		for (j = 0; j < n; j++)
			proc_anova_fit1(x, j); // fit next column
		df[i] = npar;
		if (gstate == -1)
			proc_anova_compute_G();
		proc_anova_compute_B();
		proc_anova_compute_Yhat();
		proc_anova_compute_ss();
		ss[i] = ssr;
	}

	// differentials

	for (i = nx - 1; i > 0; i--) {
		df[i] -= df[i - 1];
		ss[i] -= ss[i - 1];
	}

	df[0]--; // subtract 1 for intercept
}

void
proc_anova_fit1(int x, int level)
{
	int i, k = 0;

	for (i = 0; i < dataset->nobs; i++) {
		if (miss[i])
			continue;
		X(k++, npar) = (dataset->spec[x].v[i] == level) ? 1 : 0;
	}

	npar++;

	gstate = proc_anova_compute_G();

	if (gstate == -1)
		npar--; // X'X is singular hence remove column
}

int
proc_anova_compute_G(void)
{
	int d, i, j, k;
	double m, max, min, t;

	min = 0;
	max = 0;

	// G = I

	for (i = 0; i < npar; i++)
		for (j = 0; j < npar; j++)
			if (i == j)
				G(i, j) = 1;
			else
				G(i, j) = 0;

	// T = X'X

	for (i = 0; i < npar; i++)
		for (j = 0; j < npar; j++) {
			t = 0;
			for (k = 0; k < nobs; k++)
				t += X(k, i) * X(k, j);
			T(i, j) = t;
		}

	// G = inv T

	for (d = 0; d < npar; d++) {

		// find the best pivot row

		k = d;
		for (i = d + 1; i < npar; i++)
			if (fabs(T(i, d)) > fabs(T(k, d)))
				k = i;

		// exchange rows if necessary

		if (k != d) {
			for (j = d; j < npar; j++) { // skip zeroes, start at d
				t = T(d, j);
				T(d, j) = T(k, j);
				T(k, j) = t;
			}
			for (j = 0; j < npar; j++) {
				t = G(d, j);
				G(d, j) = G(k, j);
				G(k, j) = t;
			}
		}

		// multiply the pivot row by 1 / pivot

		m = T(d, d);

		if (m == 0)
			return -1;

		if (fabs(m) > max)
			max = fabs(m);

		m = 1 / m;

		if (fabs(m) > min)
			min = fabs(m);

		for (j = d; j < npar; j++) // skip zeroes, start at d
			T(d, j) *= m;
		for (j = 0; j < npar; j++)
			G(d, j) *= m;

		// clear out column below d

		for (i = d + 1; i < npar; i++) {
			m = -T(i, d);
			for (j = d; j < npar; j++) // skip zeroes, start at d
				T(i, j) += m * T(d, j);
			for (j = 0; j < npar; j++)
				G(i, j) += m * G(d, j);
		}
	}

	// clear out columns above diagonal

	for (d = npar - 1; d > 0; d--)
		for (i = 0; i < d; i++) {
			m = -T(i, d);
			for (j = 0; j < npar; j++)
				G(i, j) += m * G(d, j);
		}

	// check ratio of biggest divisor to smallest divisor

	// domain is [1, inf)

	// printf("cond = %g\n", max * min);

	if (max * min < 1e10)
		return 0;
	else
		return -1;
}

// B = G * X^T * Y

void
proc_anova_compute_B(void)
{
	int i, j;
	double t;

	for (i = 0; i < npar; i++) {
		t = 0;
		for (j = 0; j < nobs; j++)
			t += X(j, i) * Y[j];
		T(0, i) = t;
	}

	for (i = 0; i < npar; i++) {
		t = 0;
		for (j = 0; j < npar; j++)
			t += G(i, j) * T(0, j);
		B[i] = t;
	}
}

// Yhat = X * B

void
proc_anova_compute_Yhat(void)
{
	int i, j;
	double t;

	for (i = 0; i < nobs; i++) {
		t = 0;
		for (j = 0; j < npar; j++)
			t += X(i, j) * B[j];
		Yhat[i] = t;
	}
}

void
proc_anova_compute_ss(void)
{
	int i;

	ssr = 0;
	sse = 0;

	for (i = 0; i < nobs; i++) {
		ssr += (Yhat[i] - ybar) * (Yhat[i] - ybar);
		sse += (Y[i] - Yhat[i]) * (Y[i] - Yhat[i]);
	}
}

#define A(i, j) (a + 6 * (i))[j]

void
print_anova_table_part1(void)
{
	char **a;

	a = (char **) xmalloc(4 * 6 * sizeof (char *));

	// 1st row

	A(0, 0) = strdup("Source");
	A(0, 1) = strdup("DF");
	A(0, 2) = strdup("Sum of Squares");
	A(0, 3) = strdup("Mean Square");
	A(0, 4) = strdup("F Value");
	A(0, 5) = strdup("Pr > F");

	// 2nd row

	A(1, 0) = strdup("Model");

	snprintf(buf, sizeof buf, "%d", npar - 1);
	A(1, 1) = strdup(buf);

	snprintf(buf, sizeof buf, "%0.8f", ssr);
	A(1, 2) = strdup(buf);

	snprintf(buf, sizeof buf, "%0.8f", msr);
	A(1, 3) = strdup(buf);

	snprintf(buf, sizeof buf, "%0.2f", fval);
	A(1, 4) = strdup(buf);

	snprintf(buf, sizeof buf, "%0.4f", pval);
	A(1, 5) = strdup(buf);

	// 3rd row

	A(2, 0) = strdup("Error");

	snprintf(buf, sizeof buf, "%d", nobs - npar);
	A(2, 1) = strdup(buf);

	snprintf(buf, sizeof buf, "%0.8f", sse);
	A(2, 2) = strdup(buf);

	snprintf(buf, sizeof buf, "%0.8f", mse);
	A(2, 3) = strdup(buf);

	A(2, 4) = strdup("");
	A(2, 5) = strdup("");

	// 4th row

	A(3, 0) = strdup("Total");

	snprintf(buf, sizeof buf, "%d", nobs - 1);
	A(3, 1) = strdup(buf);

	snprintf(buf, sizeof buf, "%0.8f", css);
	A(3, 2) = strdup(buf);

	A(3, 3) = strdup("");
	A(3, 4) = strdup("");
	A(3, 5) = strdup("");

	buf[0] = 1; // left justify
	buf[1] = 0;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 0;

	print_table_and_free(a, 4, 6, buf);
}

#undef A
#define A(i, j) (a + 4 * (i))[j]

void
print_anova_table_part2(void)
{
	char **a;

	a = (char **) xmalloc(8 * sizeof (char *));

	A(0, 0) = strdup("R-Square");
	A(0, 1) = strdup("Coeff Var");
	A(0, 2) = strdup("Root MSE");

	snprintf(buf, sizeof buf, "%s Mean", dataset->spec[yvar].name);
	A(0, 3) = strdup(buf);

	snprintf(buf, sizeof buf, "%0.6f", rsquare);
	A(1, 0) = strdup(buf);

	snprintf(buf, sizeof buf, "%0.6f", cv);
	A(1, 1) = strdup(buf);

	snprintf(buf, sizeof buf, "%0.6f", rootmse);
	A(1, 2) = strdup(buf);

	snprintf(buf, sizeof buf, "%0.6f", ybar);
	A(1, 3) = strdup(buf);

	buf[0] = 0; // right justified
	buf[1] = 0;
	buf[2] = 0;
	buf[3] = 0;

	print_table_and_free(a, 2, 4, buf);
}

#undef A
#define A(i, j) (a + 6 * (i))[j]

void
print_anova_table_part3(void)
{
	int i, x;
	double msq, fval, pval;
	char **a;

	a = (char **) xmalloc(6 * (nx + 1) * sizeof (char *));

	A(0, 0) = strdup("Source");
	A(0, 1) = strdup("DF");
	A(0, 2) = strdup("Anova SS");
	A(0, 3) = strdup("Mean Square");
	A(0, 4) = strdup("F Value");
	A(0, 5) = strdup("Pr > F");

	for (i = 0; i < nx; i++) {

		x = xtab[i];

		// Source

		A(i + 1, 0) = strdup(dataset->spec[x].name);

		// DF

		snprintf(buf, sizeof buf, "%d", df[i]);
		A(i + 1, 1) = strdup(buf);

		if (df[i] == 0) {
			A(i + 1, 2) = strdup(".");
			A(i + 1, 3) = strdup(".");
			A(i + 1, 4) = strdup(".");
			A(i + 1, 5) = strdup(".");
			continue;
		}

		// Anova SS

		snprintf(buf, sizeof buf, "%0.8f", ss[i]);
		A(i + 1, 2) = strdup(buf);

		// Mean Square

		msq = ss[i] / df[i];

		snprintf(buf, sizeof buf, "%0.8f", msq);
		A(i + 1, 3) = strdup(buf);

		// F Value

		fval = msq / mse;

		snprintf(buf, sizeof buf, "%0.2f", fval);
		A(i + 1, 4) = strdup(buf);

		// Pr > F

		pval = 1 - fdist(fval, df[i], nobs - npar);

		snprintf(buf, sizeof buf, "%0.4f", pval);
		A(i + 1, 5) = strdup(buf);
	}

	buf[0] = 1; // left justify
	buf[1] = 0;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 0;

	print_table_and_free(a, nx + 1, 6, buf);
}

void
proc_anova_compute_means(int x)
{
	int i, level, n;
	double t, y;

	n = dataset->spec[x].num_levels;

	if (n > MAXLVL)
		stop("Too many levels");

	for (i = 0; i < n; i++) {
		mean[i] = 0;
		variance[i] = 0;
		count[i] = 0;
	}

	for (i = 0; i < dataset->nobs; i++) {
		if (miss[i])
			continue;
		y = dataset->spec[yvar].v[i];
		level = dataset->spec[x].v[i];
		count[level]++;
		t = mean[level];
		mean[level] += (y - t) / count[level];
		variance[level] += (y - t) * (y - mean[level]);
	}

	for (i = 0; i < n; i++)
		variance[i] /= (count[i] - 1);
}

#undef A
#define A(i, j) (a + (5 + m) * (i))[j]

void
proc_anova_print_means(int x)
{
	int i, j, m, n;
	double q, t;
	char **a, *b, *s;

	q = qt(1 - alpha / 2, dfe);

	emit_line_center("Mean Response");
	emit_line("");

	// count the number of interactions

	m = 0;
	s = dataset->spec[x].name;
	while (*s)
		if (*s++ == '*')
			m++;

	n = dataset->spec[x].num_levels;

	a = (char **) xmalloc((5 + m) * (n + 1) * sizeof (char *));

	// split up interaction string

	s = dataset->spec[x].name;
	for (i = 0; i < m + 1; i++) {
		b = buf;
		while (*s && *s != '*')
			*b++ = *s++;
		if (*s)
			s++;
		*b = 0;
		A(0, i) = strdup(buf);
	}

	A(0, m + 1) = strdup("N");

	s = dataset->spec[yvar].name;
	snprintf(buf, sizeof buf, "Mean %s", s);
	A(0, m + 2) = strdup(buf);

	snprintf(buf, sizeof buf, "%g%% CI MIN", 100 * (1 - alpha));
	A(0, m + 3) = strdup(buf);

	snprintf(buf, sizeof buf, "%g%% CI MAX", 100 * (1 - alpha));
	A(0, m + 4) = strdup(buf);

	for (i = 0; i < n; i++) {

		// Level (decompose interactions)

		s = dataset->spec[x].ltab[i];
		for (j = 0; j < m + 1; j++) {
			b = buf;
			while (*s && *s != '*')
				*b++ = *s++;
			if (*s)
				s++;
			*b = 0;
			A(i + 1, j) = strdup(buf);
		}

		// N

		snprintf(buf, sizeof buf, "%d", count[i]);
		A(i + 1, m + 1) = strdup(buf);

		if (count[i] < 1) {
			A(i + 1, m + 2) = strdup(".");
			A(i + 1, m + 3) = strdup(".");
			A(i + 1, m + 4) = strdup(".");
			continue;
		}

		// Mean

		snprintf(buf, sizeof buf, "%0.6f", mean[i]);
		A(i + 1, m + 2) = strdup(buf);

		// Confidence Interval

		t = q * sqrt(mse / count[i]);

		snprintf(buf, sizeof buf, "%0.6f", mean[i] - t);
		A(i + 1, m + 3) = strdup(buf);

		snprintf(buf, sizeof buf, "%0.6f", mean[i] + t);
		A(i + 1, m + 4) = strdup(buf);
	}

	for (i = 0; i < m + 1; i++)
		buf[i] = 1;
	buf[m + 1] = 0;
	buf[m + 2] = 0;
	buf[m + 3] = 0;
	buf[m + 4] = 0;

	print_table_and_free(a, n + 1, 5 + m, buf);
}

#undef A
#define A(i, j) (a + ncol * (i))[j]

void
proc_anova_print_lsd(int x)
{
	int i, j, k, n, ncol, nrow;
	char **a, *s;
	double d, lsd, pval, q, se, tval;

	emit_line_center("Least Significant Difference Test");
	emit_line("");

	q = qt(1 - alpha / 2, dfe); // degrees of freedom error

	n = dataset->spec[x].num_levels;

	nrow = 1 + n * (n - 1);

	ncol = 7;

	a = (char **) xmalloc(nrow * ncol * sizeof (char *));

	s = dataset->spec[x].name;
	A(0, 0) = strdup(s);
	A(0, 1) = strdup(s);

	s = dataset->spec[yvar].name;
	snprintf(buf, sizeof buf, "Delta %s", s);
	A(0, 2) = strdup(buf);

	snprintf(buf, sizeof buf, "%g%% CI MIN", 100 * (1 - alpha));
	A(0, 3) = strdup(buf);

	snprintf(buf, sizeof buf, "%g%% CI MAX", 100 * (1 - alpha));
	A(0, 4) = strdup(buf);

	A(0, 5) = strdup("t Value");
	A(0, 6) = strdup("Pr > |t|  ");

	k = 0;

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {

			if (i == j)
				continue;

			k++;

			// Level

			s = dataset->spec[x].ltab[i];
			A(k, 0) = strdup(s);

			// Level

			s = dataset->spec[x].ltab[j];
			A(k, 1) = strdup(s);

			// Sanity check

			if (count[i] == 0 || count[j] == 0) {
				A(k, 2) = strdup(".");
				A(k, 3) = strdup(".");
				A(k, 4) = strdup(".");
				A(k, 5) = strdup(".");
				A(k, 6) = strdup(".  ");
				continue;
			}

			// Difference

			d = mean[i] - mean[j];
			snprintf(buf, sizeof buf, "%0.6f", d);
			A(k, 2) = strdup(buf);

			// confidence interval

			se = sqrt(mse * (1.0 / count[i] + 1.0 / count[j]));

			lsd = q * se;

			tval = d / se;

			pval = 2 * (1 - tdist(fabs(tval), dfe));

			snprintf(buf, sizeof buf, "%0.6f", d - lsd);
			A(k, 3) = strdup(buf);

			snprintf(buf, sizeof buf, "%0.6f", d + lsd);
			A(k, 4) = strdup(buf);

			// t Value

			snprintf(buf, sizeof buf, "%0.2f", tval);
			A(k, 5) = strdup(buf);

			// Pr > |t|

			if (pval > alpha)
				snprintf(buf, sizeof buf, "%0.4f  ", pval);
			else
				snprintf(buf, sizeof buf, "%0.4f *", pval);
			A(k, 6) = strdup(buf);
		}
	}

	buf[0] = 1; // left justify
	buf[1] = 1;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 0;
	buf[6] = 0;

	print_table_and_free(a, nrow, ncol, buf);
}

void
proc_anova_print_ttest(int x)
{
	int dfe, i, j, k, n, ncol, nrow;
	char **a, *s;
	double d, mse, pval, se, sse, t, tval;

	emit_line_center("Two Sample t-Test");
	emit_line("");

	n = dataset->spec[x].num_levels;

	nrow = 1 + n * (n - 1);

	ncol = 7;

	a = (char **) xmalloc(nrow * ncol * sizeof (char *));

	s = dataset->spec[x].name;
	A(0, 0) = strdup(s);
	A(0, 1) = strdup(s);

	s = dataset->spec[yvar].name;
	snprintf(buf, sizeof buf, "Delta %s", s);
	A(0, 2) = strdup(buf);

	snprintf(buf, sizeof buf, "%g%% CI MIN", 100 * (1 - alpha));
	A(0, 3) = strdup(buf);

	snprintf(buf, sizeof buf, "%g%% CI MAX", 100 * (1 - alpha));
	A(0, 4) = strdup(buf);

	A(0, 5) = strdup("t Value");
	A(0, 6) = strdup("Pr > |t|  ");

	k = 0;

	for (i = 0; i < n; i++) {
		for (j = 0; j < n; j++) {

			if (i == j)
				continue;

			k++;

			// Level

			s = dataset->spec[x].ltab[i];
			A(k, 0) = strdup(s);

			// Level

			s = dataset->spec[x].ltab[j];
			A(k, 1) = strdup(s);

			// Sanity check

			if (count[i] == 0 || count[j] == 0) {
				A(k, 2) = strdup(".");
				A(k, 3) = strdup(".");
				A(k, 4) = strdup(".");
				A(k, 5) = strdup(".");
				A(k, 6) = strdup(".  ");
				continue;
			}

			// Difference

			d = mean[i] - mean[j];
			snprintf(buf, sizeof buf, "%0.6f", d);
			A(k, 2) = strdup(buf);

			// confidence interval

			if (count[i] + count[j] < 3) {
				A(k, 3) = strdup(".");
				A(k, 4) = strdup(".");
				A(k, 5) = strdup(".");
				A(k, 6) = strdup(".  ");
				continue;
			}

			sse = variance[i] * (count[i] - 1) + variance[j] * (count[j] - 1);

			dfe = count[i] + count[j] - 2;

			mse = sse / dfe;

			se = sqrt(mse * (1.0 / count[i] + 1.0 / count[j]));

			tval = d / se;

			pval = 2 * (1 - tdist(fabs(tval), dfe));

			t = qt(1 - alpha / 2, dfe) * se;

			snprintf(buf, sizeof buf, "%0.6f", d - t);
			A(k, 3) = strdup(buf);

			snprintf(buf, sizeof buf, "%0.6f", d + t);
			A(k, 4) = strdup(buf);

			snprintf(buf, sizeof buf, "%0.2f", tval);
			A(k, 5) = strdup(buf);

			if (pval > alpha)
				snprintf(buf, sizeof buf, "%0.4f  ", pval);
			else
				snprintf(buf, sizeof buf, "%0.4f *", pval);
			A(k, 6) = strdup(buf);
		}
	}

	buf[0] = 1; // left justify
	buf[1] = 1;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 0;
	buf[6] = 0;

	print_table_and_free(a, nrow, ncol, buf);
}

#undef G
#undef T
#undef X
#undef A
char **a;
char tblfmt[MAXVAR + MAXSTAT + 1];
int row;
int filtertab[MAXVAR];

void
proc_means(void)
{
	parse_proc_means_stmt();

	if (dataset == NULL)
		stop("No data set");

	parse_proc_means_body();

	print_title();

	run_proc_means();
}

void
parse_proc_means_stmt(void)
{
	for (;;) {
		scan();
		keyword();
		switch (token) {
		case ';':
			scan(); // eat the semicolon
			return;
		case KALPHA:
			parse_alpha_option();
			break;
		case KDATA:
			parse_data_option();
			break;
		case KMAXDEC:
			parse_maxdec_option();
			break;
		case KCLM:
			if (nstats + 2 > MAXSTAT)
				stop("Too many statistics keywords");
			stats[nstats++] = KCLM1;
			stats[nstats++] = KCLM2;
			break;
		case KLCLM:
		case KMAX:
		case KMEAN:
		case KMIN:
		case KN:
		case KSUM:
		case KSTD:
		case KSTDDEV:
		case KSTDERR:
		case KSTDMEAN:
		case KUCLM:
		case KVAR:
			if (nstats == MAXSTAT)
				stop("Too many statistics keywords");
			stats[nstats++] = token;
			break;
		default:
			expected("proc means option");
		}
	}
}

void
parse_proc_means_body(void)
{
	for (;;) {
		keyword();
		switch (token) {
		case 0:
		case KDATA:
		case KPROC:
		case KRUN:
			return;
		case KCLASS:
			parse_class_stmt();
			break;
		case KVAR:
			parse_var_stmt();
			break;
		default:
			parse_default();
			break;
		}
	}
}

#define A(i, j) (a + (i) * ncol)[j]

void
run_proc_means(void)
{
	int i, j, k, n;
	char *t;
	char buf[100];

	// print all numeric variables if not specified with VAR

	if (nvar == 0) {
		k = 0;
		for (i = 0; i < dataset->nvar; i++)
			if (dataset->spec[i].ltab == NULL)
				var[k++] = i;
		nvar = k;
	}

	// default statistics

	if (nstats == 0) {
		nstats = 5;
		stats[0] = KN;
		stats[1] = KMEAN;
		stats[2] = KSTD;
		stats[3] = KMIN;
		stats[4] = KMAX;
	}

	ncol = nclass + nstats + 1;

	nrow = nvar;

	for (i = 0; i < nclass; i++) {
		k = class[i];
		n = dataset->spec[k].num_levels;
		nrow *= n;
	}

	nrow++; // for header row

	a = (char **) xmalloc(nrow * ncol * sizeof (char *));

	// table header line

	// categorical variable names, if any

	for (i = 0; i < nclass; i++) {
		k = class[i];
		t = dataset->spec[k].name;
		A(0, i) = strdup(t);
	}

	// variable name column

	A(0, nclass) = strdup("Variable");

	// statistic names

	for (i = 0; i < nstats; i++) {
		switch (stats[i]) {
		case KCLM1:
			snprintf(buf, sizeof buf, "%g%% CLM MIN", 100 * (1 - alpha));
			t = buf;
			break;
		case KCLM2:
			snprintf(buf, sizeof buf, "%g%% CLM MAX", 100 * (1 - alpha));
			t = buf;
			break;
		case KLCLM:
			snprintf(buf, sizeof buf, "%g%% LCLM", 100 * (1 - alpha));
			t = buf;
			break;
		case KMAX:
			t = "Maximum";
			break;
		case KMEAN:
			t = "Mean";
			break;
		case KMIN:
			t = "Minimum";
			break;
		case KN:
			t = "N";
			break;
		case KRANGE:
			t = "Range";
			break;
		case KSUM:
			t = "Sum";
			break;
		case KSTD:
		case KSTDDEV:
			t = "Std Dev";
			break;
		case KSTDERR:
		case KSTDMEAN:
			t = "Std Err";
			break;
		case KUCLM:
			snprintf(buf, sizeof buf, "%g%% UCLM", 100 * (1 - alpha));
			t = buf;
			break;
		case KVAR:
			t = "Variance";
			break;
		default:
			t = ".";
			break;
		}
		A(0, nclass + 1 + i) = strdup(t);
	}

	row = 1;

	f(0);

	// right justify variable names

	for (i = 0; i < ncol; i++) {
		if (i < nclass + 1)
			tblfmt[i] = 1;
		else
			tblfmt[i] = 0;
	}

	print_table(a, nrow, ncol, tblfmt);

	for (i = 0; i < nrow; i++)
		for (j = 0; j < ncol; j++)
			free(A(i, j));

	free(a);
}

// k is the index into class[]

void
f(int k)
{
	int i, n, varnum;

	if (k == nclass) {
		g();
		return;
	}

	varnum = class[k];

	// number of levels

	n = dataset->spec[varnum].num_levels;

	// for each level...

	for (i = 0; i < n; i++) {
		filtertab[k] = i;
		f(k + 1);
	}
}

// to here after category filter is set up

void
g(void)
{
	int i, j, level, varnum;

	// for each numeric variable...

	for (i = 0; i < nvar; i++) {

		// level names

		if (i == 0)
			for (j = 0; j < nclass; j++) {
				varnum = class[j];
				level = filtertab[j];
				A(row, j) = strdup(dataset->spec[varnum].ltab[level]);
			}
		else
			for (j = 0; j < nclass; j++)
				A(row, j) = strdup("");

		varnum = var[i];

		// name of the variable

		A(row, nclass) = strdup(dataset->spec[varnum].name);

		h(varnum);

		row++;
	}
}

// one row of statistics for varnum

void
h(int varnum)
{
	int i, j, k, w;
	double m, t1, t2, x;
	char buf[100];

	int n = 0;

	double mean = NAN;
	double variance = NAN;

	double min = NAN;
	double max = NAN;
	double sum = NAN;

	double stddev = NAN;
	double stderror = NAN;

	double range = NAN;

	for (i = 0; i < dataset->nobs; i++) {

		// filter

		for (j = 0; j < nclass; j++) {
			k = class[j];
			if (dataset->spec[k].v[i] != filtertab[j])
				break;
		}

		if (j < nclass)
			continue;

		// if categorical variable then only N makes sense

		if (dataset->spec[varnum].ltab) {
			if (dataset->spec[varnum].v[i])
				n++;
			continue;
		}

		x = dataset->spec[varnum].v[i];

		if (isnan(x))
			continue;

		n++;

		if (n == 1) {
			mean = x;
			variance = 0.0;
			min = x;
			max = x;
			sum = 0.0;
			stddev = 0.0;
			stderror = 0.0;
		}

		m = mean;

		mean += (x - m) / n;

		variance += (x - m) * (x - mean);

		if (x < min)
			min = x;

		if (x > max)
			max = x;

		sum += x;
	}

	range = max - min;

	if (n > 1) {
		variance /= (n - 1);
		stddev = sqrt(variance);
		stderror = stddev / sqrt(n);
	}

	t1 = qt(1 - alpha, n - 1.0);
	t2 = qt(1 - alpha / 2, n - 1.0);

	for (i = 0; i < nstats; i++) {
		w = maxdec;
		switch (stats[i]) {
		case KCLM1:
			x = mean - t2 * stderror;
			break;
		case KCLM2:
			x = mean + t2 * stderror;
			break;
		case KLCLM:
			x = mean - t1 * stderror;
			break;
		case KMAX:
			x = max;
			break;
		case KMEAN:
			x = mean;
			break;
		case KMIN:
			x = min;
			break;
		case KN:
			x = n;
			w = 0;
			break;
		case KRANGE:
			x = range;
			break;
		case KSUM:
			x = sum;
			break;
		case KSTD:
		case KSTDDEV:
			x = stddev;
			break;
		case KSTDERR:
		case KSTDMEAN:
			x = stderror;
			break;
		case KUCLM:
			x = mean + t1 * stderror;
			break;
		case KVAR:
			x = variance;
			break;
		default:
			x = NAN;
			break;
		}
		if (isnan(x))
			A(row, nclass + 1 + i) = strdup(".");
		else {
			snprintf(buf, sizeof buf, fmt[w], x);
			A(row, nclass + 1 + i) = strdup(buf);
		}
	}
}

#undef A
void
proc_print(void)
{
	parse_proc_print_stmt();

	if (dataset == NULL)
		stop("No data set");

	parse_proc_print_body();

	run_proc_print();
}

void
parse_proc_print_stmt(void)
{
	for (;;) {
		get_next_token();
		switch (token) {
		case ';':
			scan();
			return;
		case KDATA:
			parse_data_option();
			break;
		default:
			stop("';' expected");
		}
	}
}

// parse statements that follow PROC PRINT

void
parse_proc_print_body(void)
{
	for (;;) {
		keyword();
		switch (token) {
		case 0:
		case KDATA:
		case KPROC:
		case KRUN:
			return;
		case KVAR:
			parse_var_stmt();
			break;
		default:
			parse_default();
			break;
		}
	}
}

#define A(i, j) (a + (i) * (nvar + 1))[j]

void
run_proc_print(void)
{
	int i, j, nobs, w, x;
	double d;
	char **a, *s;

	print_title();

	nobs = dataset->nobs;

	// print all variables if not specified with VAR

	if (nvar == 0) {
		nvar = dataset->nvar;
		for (i = 0; i < nvar; i++)
			var[i] = i;
	}

	// create array

	a = (char **) xmalloc((nobs + 1) * (nvar + 1) * sizeof (char *));

	// column names

	A(0, 0) = strdup("Obs");

	for (j = 0; j < nvar; j++) {
		x = var[j];
		A(0, j + 1) = strdup(dataset->spec[x].name);
	}

	// for each row

	for (i = 0; i < nobs; i++) {

		// observation number

		snprintf(buf, sizeof buf, "%d", i + 1);

		A(i + 1, 0) = strdup(buf);

		// for each variable

		for (j = 0; j < nvar; j++) {

			x = var[j];

			d = dataset->spec[x].v[i];

			if (isnan(d))
				s = ".";
			else {
				if (dataset->spec[x].ltab == NULL) {
					w = dataset->spec[x].w;
					snprintf(buf, sizeof buf, fmt[w], d);
					s = buf;
				} else
					s = dataset->spec[x].ltab[(int) d];
			}

			A(i + 1, j + 1) = strdup(s);
		}
	}

	s = (char *) xmalloc(nvar + 1);

	s[0] = 0; // observation numbers are right justified

	for (j = 0; j < nvar; j++) {
		x = var[j];
		if (dataset->spec[x].ltab == NULL)
			s[j + 1] = 0;
		else
			s[j + 1] = 1;
	}

	print_table(a, nobs + 1, nvar + 1, s);

	free(s);

	for (i = 0; i < nobs + 1; i++)
		for (j = 0; j < nvar + 1; j++)
			free(A(i, j));

	free(a);
}

#undef A
//	B	Regression coefficients
//
//	G	Inverse of X'X
//
//	T	Temporary matrix
//
//	X	Design matrix
//
//	Y	Response vector
//
//
//
//	alpha	Level of significance
//
//	css	Corrected sum of squares (same as sst)
//
//	df	Degrees of freedom
//
//	dfe	Degrees of freedom error
//
//	dfm	Degrees of freedom model
//
//	dft	Degrees of freedom total
//
//	fval	Summary F-statistic
//
//	mse	Mean square error (estimate of model variance)
//
//	msr	Mean square regression
//
//	pval	p-value for F-statistic
//
//	ss	Sequential sum of squares (Type I)
//
//	ssr	Sum of squares regression
//
//	sse	Sum of squares error
//
//	sst	Sum of squares total
//
//	ybar	Mean of response variable Y
//
//
//
//	adjrsq	Adjusted R-squared
//
//	cv	Coefficient of variation
//
//	rsquare	R-squared
//
//	rootmse	Square root of MSE

void
proc_reg(void)
{
	num_x = 0;
	num_y = 0;

	proc_reg_parse_stmt();

	if (dataset == NULL)
		stop("No data set");

	proc_reg_parse_body();

	proc_reg_regress();

	print_title();

	emit_line_center("Analysis of Variance");
	emit_line("");

	proc_reg_print_anova_table();
	proc_reg_print_diag_table();

	emit_line_center("Parameter Estimates");
	emit_line("");

	proc_reg_print_parameter_estimates();
}

void
proc_reg_parse_stmt(void)
{
	for (;;) {
		scan();
		keyword();
		switch (token) {
		case ';':
			scan(); // eat the semicolon
			return;
		case KALPHA:
			parse_alpha_option();
			break;
		case KDATA:
			parse_data_option();
			break;
		default:
			expected("proc reg option");
		}
	}
}

void
proc_reg_parse_body(void)
{
	for (;;) {
		keyword();
		switch (token) {
		case 0:
		case KDATA:
		case KPROC:
		case KRUN:
			return;
		case KMODEL:
			proc_reg_parse_model_stmt();
			break;
		default:
			parse_default();
			break;
		}
	}
}

void
proc_reg_parse_model_stmt(void)
{
	int i;

	noint = 0;

	num_x = 0;
	num_y = 0;

	scan();

	if (token != NAME)
		expected("variable name");

	// look for match in dataset

	for (i = 0; i < dataset->nvar; i++)
		if (strcmp(dataset->spec[i].name, strbuf) == 0)
			break;

	if (i == dataset->nvar) {
		snprintf(errbuf, ERRBUFLEN, "Variable %s not in dataset", strbuf);
		stop(errbuf);
	}

	ytab[num_y++] = i;

	scan();

	if (token != '=')
		expected("equals sign");

	for (;;) {

		scan();

		if ((token == ';' || token == '/') && num_x)
			break;

		if (token != NAME)
			expected("variable name");

		// look for match in dataset

		for (i = 0; i < dataset->nvar; i++)
			if (strcmp(dataset->spec[i].name, strbuf) == 0)
				break;

		if (i == dataset->nvar) {
			snprintf(errbuf, ERRBUFLEN, "Variable %s not in dataset", strbuf);
			stop(errbuf);
		}

		xtab[num_x++] = i;
	}

	if (token == '/')
		proc_reg_parse_model_options();

	scan(); // eat the semicolon
}

void
proc_reg_parse_model_options(void)
{
	for (;;) {

		scan();

		keyword();

		switch (token) {

		case ';':
			return;

		case KNOINT:
			noint = 1;
			break;

		default:
			stop("';' or MODEL option expected");
		}
	}
}

#define C(i, j) (CC + (i) * ncol)[j]
#define G(i, j) (GG + (i) * ncol)[j]
#define T(i, j) (TT + (i) * ncol)[j]
#define X(i, j) (XX + (i) * ncol)[j]

// X is the design matrix

void
proc_reg_compute_X(void)
{
	int i, j, k, l, m, n, x, y;
	double v;

	l = 0;

	for (i = 0; i < nrow; i++) {

		// check for missing data

		y = ytab[0];

		v = dataset->spec[y].v[i];

		if (isnan(v))
			continue;

		Y[l] = v;

		if (noint)
			m = 0;
		else {
			m = 1;
			X(l, 0) = 1;
		}

		for (j = 0; j < num_x; j++) {

			x = xtab[j];

			v = dataset->spec[x].v[i];

			if (isnan(v))
				break;

			if (dataset->spec[x].ltab == NULL)
				X(l, m++) = v;
			else {

				// categorical variable

				n = dataset->spec[x].num_levels;

				for (k = 0; k < n; k++)
					if (k == v)
						X(l, m + k) = 1;
					else
						X(l, m + k) = 0;

				m += n;
			}
		}

		if (j == num_x)
			l++; // full row, no nans
	}

	// missing data may reduce nrow

	nrow = l;
}

// T = X^T * X

void
proc_reg_compute_T(void)
{
	int i, j, k, l, m;
	double t;
	l = 0;
	for (i = 0; i < ncol; i++) {
		if (Z[i])
			continue;
		m = 0;
		for (j = 0; j < ncol; j++) {
			if (Z[j])
				continue;
			t = 0;
			for (k = 0; k < nrow; k++)
				t += X(k, i) * X(k, j);
			T(l, m) = t;
			m++;
		}
		l++;
	}
}

// G = T ^ (-1)

// T is clobbered

int
proc_reg_compute_G(void)
{
	int d, i, j, k;
	double m, max, min, t;

	min = 0;
	max = 0;

	// G = I

	for (i = 0; i < npar; i++)
		for (j = 0; j < npar; j++)
			if (i == j)
				G(i, j) = 1;
			else
				G(i, j) = 0;

	for (d = 0; d < npar; d++) {

		// find the best pivot row

		k = d;
		for (i = d + 1; i < npar; i++)
			if (fabs(T(i, d)) > fabs(T(k, d)))
				k = i;

		// exchange rows if necessary

		if (k != d) {
			for (j = d; j < npar; j++) { // skip zeroes, start at d
				t = T(d, j);
				T(d, j) = T(k, j);
				T(k, j) = t;
			}
			for (j = 0; j < npar; j++) {
				t = G(d, j);
				G(d, j) = G(k, j);
				G(k, j) = t;
			}
		}

		// multiply the pivot row by 1 / pivot

		m = T(d, d);

		if (m == 0)
			return -1;

		if (fabs(m) > max)
			max = fabs(m);

		m = 1 / m;

		if (fabs(m) > min)
			min = fabs(m);

		for (j = d; j < npar; j++) // skip zeroes, start at d
			T(d, j) *= m;
		for (j = 0; j < npar; j++)
			G(d, j) *= m;

		// clear out column below d

		for (i = d + 1; i < npar; i++) {
			m = -T(i, d);
			for (j = d; j < npar; j++) // skip zeroes, start at d
				T(i, j) += m * T(d, j);
			for (j = 0; j < npar; j++)
				G(i, j) += m * G(d, j);
		}
	}

	// clear out columns above diagonal

	for (d = npar - 1; d > 0; d--)
		for (i = 0; i < d; i++) {
			m = -T(i, d);
			for (j = 0; j < npar; j++)
				G(i, j) += m * G(d, j);
		}

	// check ratio of biggest divisor to smallest divisor

	// domain is [1, inf)

	// printf("cond = %g\n", max * min);

	if (max * min < 1e10)
		return 0;
	else
		return -1;
}

// B = G * X^T * Y

void
proc_reg_compute_B(void)
{
	int i, j, l;
	double t;

	l = 0;

	for (i = 0; i < ncol; i++) {
		if (Z[i])
			continue;
		t = 0;
		for (j = 0; j < nrow; j++)
			t += X(j, i) * Y[j];
		T(0, l++) = t;
	}

	for (i = 0; i < npar; i++) {
		t = 0;
		for (j = 0; j < npar; j++)
			t += G(i, j) * T(0, j);
		B[i] = t;
	}
}

// E = Y - X * B

// sse = E^T * E

// mse = sse / (nrow - npar)

void
proc_reg_compute_mse(void)
{
	int i, j, k;
	double yhat;

	dfm = npar - 1;
	dfe = nrow - npar;
	dft = nrow - 1;

	ybar = 0;

	for (i = 0; i < nrow; i++)
		ybar += (Y[i] - ybar) / (i + 1);

	ssr = 0;
	sse = 0;
	sst = 0;

	for (i = 0; i < nrow; i++) {
		k = 0;
		yhat = 0;
		for (j = 0; j < ncol; j++)
			if (Z[j] == 0)
				yhat += X(i, j) * B[k++];
		ssr += (yhat - ybar) * (yhat - ybar);
		sse += (Y[i] - yhat) * (Y[i] - yhat);
		sst += (Y[i] - ybar) * (Y[i] - ybar);
	}

	mse = sse / dfe;

	rootmse = sqrt(mse);

	msr = ssr / dfm;

	fval = msr / mse;

	pval = 1.0 - fdist(fval, dfm, dfe);

	rsquare = 1.0 - sse / sst;

	adjrsq = 1.0 - dft / dfe * sse / sst;

	cv = 100.0 * rootmse / ybar;
}

// C = mse * G

void
proc_reg_compute_C(void)
{
	int i, j;
	for (i = 0; i < npar; i++)
		for (j = 0; j < npar; j++)
			C(i, j) = mse * G(i, j);
}

// SE[i] = sqrt(C[i][i])

void
proc_reg_compute_SE(void)
{
	int i;
	for (i = 0; i < npar; i++)
		SE[i] = sqrt(C(i, i));
}

void
proc_reg_compute_TVAL(void)
{
	int i;
	for (i = 0; i < npar; i++)
		TVAL[i] = B[i] / SE[i];
}

void
proc_reg_compute_PVAL(void)
{
	int i, n;
	n = nrow - npar;
	for (i = 0; i < npar; i++)
		PVAL[i] = 2 * (1 - tdist(fabs(TVAL[i]), n));
}

void
proc_reg_print_B(void)
{
	int i;
	printf("B =\n");
	for (i = 0; i < npar; i++)
		printf("%20g\n", B[i]);
}

void
proc_reg_print_X(void)
{
	int i, j;
	printf("X =\n");
	for (i = 0; i < nrow; i++) {
		for (j = 0; j < ncol; j++)
			printf("%20g", X(i, j));
		printf("\n");
	}
}

void
proc_reg_print_T(void)
{
	int i, j;
	printf("T =\n");
	for (i = 0; i < npar; i++) {
		for (j = 0; j < npar; j++)
			printf("%20g", T(i, j));
		printf("\n");
	}
}

void
proc_reg_print_G(void)
{
	int i, j;
	printf("G =\n");
	for (i = 0; i < npar; i++) {
		for (j = 0; j < npar; j++)
			printf("%20g", G(i, j));
		printf("\n");
	}
}

void
proc_reg_print_Z(void)
{
	int i;
	printf("Z =\n");
	for (i = 0; i < ncol; i++)
		printf("%20d\n", Z[i]);
}

void
proc_reg_regress(void)
{
	int i, x;

	nrow = dataset->nobs;

	// determine the number of design matrix columns

	if (noint)
		ncol = 0;
	else
		ncol = 1;

	for (i = 0; i < num_x; i++) {
		x = xtab[i];
		if (dataset->spec[x].ltab == NULL)
			ncol++;
		else
			ncol += dataset->spec[x].num_levels;
	}

	FREE(Z)
	FREE(Y)
	FREE(B)
	FREE(SE)
	FREE(TVAL)
	FREE(PVAL)
	FREE(CC)
	FREE(GG)
	FREE(TT)
	FREE(XX)

	Z = xmalloc(ncol * sizeof (int));

	Y = xmalloc(nrow * sizeof (double));
	B = xmalloc(ncol * sizeof (double));
	SE = xmalloc(ncol * sizeof (double));
	TVAL = xmalloc(ncol * sizeof (double));
	PVAL = xmalloc(ncol * sizeof (double));

	CC = xmalloc(ncol * ncol * sizeof (double));
	GG = xmalloc(ncol * ncol * sizeof (double));
	TT = xmalloc(ncol * ncol * sizeof (double));
	XX = xmalloc(nrow * ncol * sizeof (double));

	proc_reg_compute_X();

	npar = ncol;

	for (i = 0; i < ncol; i++)
		Z[i] = 0;

	proc_reg_compute_T();

	// if singular then put in columns one by one

	if (proc_reg_compute_G() == -1) {
		npar = 0;
		for (i = 0; i < ncol; i++)
			Z[i] = 1;
		for (i = 0; i < ncol; i++) {
			npar++;
			Z[i] = 0;
			proc_reg_compute_T();
			if (proc_reg_compute_G() == -1) {
				npar--;
				Z[i] = 1;
			}
		}

		// did last column get zapped?

		if (Z[ncol - 1]) {
			proc_reg_compute_T();
			proc_reg_compute_G();
		}
	}

	// sanity check

	if (npar < 1 || npar >= nrow) {
		snprintf(errbuf, ERRBUFLEN, "Regression model kaput, p = %d, n = %d, must have 0 < p < n", npar, nrow);
		stop(errbuf);
	}

	proc_reg_compute_B();

	proc_reg_compute_mse();

	proc_reg_compute_C();

	proc_reg_compute_SE();

	proc_reg_compute_TVAL();

	proc_reg_compute_PVAL();
}

#define A(i, j) (a + 5 * (i))[j]

void
proc_reg_print_parameter_estimates(void)
{
	int i, j, k, m, n, x;
	char buf[100];

	m = ncol + 1;

	char **a = xmalloc(m * 5 * sizeof (char *));

	A(0, 0) = strdup("");
	A(0, 1) = strdup("Estimate");
	A(0, 2) = strdup("Std Err");
	A(0, 3) = strdup("t Value");
	A(0, 4) = strdup("Pr > |t|");

	// variable names

	k = 1;

	if (noint == 0)
		A(k++, 0) = strdup("Intercept");

	for (i = 0; i < num_x; i++) {
		x = xtab[i];
		if (dataset->spec[x].ltab == NULL)
			A(k++, 0) = strdup(dataset->spec[x].name);
		else {
			n = dataset->spec[x].num_levels;
			for (j = 0; j < n; j++) {
				snprintf(buf, sizeof buf, "%s %s", dataset->spec[x].name, dataset->spec[x].ltab[j]);
				A(k++, 0) = strdup(buf);
			}
		}
	}

	k = 0;

	for (i = 1; i < m; i++) {

		if (Z[i - 1]) {
			A(i, 1) = strdup(".");
			A(i, 2) = strdup(".");
			A(i, 3) = strdup(".");
			A(i, 4) = strdup(".");
			continue;
		}

		snprintf(buf, sizeof buf, "%0.5f", B[k]);
		A(i, 1) = strdup(buf);

		snprintf(buf, sizeof buf, "%0.5f", SE[k]);
		A(i, 2) = strdup(buf);

		snprintf(buf, sizeof buf, "%0.2f", TVAL[k]);
		A(i, 3) = strdup(buf);

		snprintf(buf, sizeof buf, "%0.4f", PVAL[k]);
		A(i, 4) = strdup(buf);

		k++;
	}

	buf[0] = 1; // right justify
	buf[1] = 0;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;

	print_table(a, m, 5, buf);

	for (i = 0; i < m; i++)
		for (j = 0; j < 5; j++)
			free(A(i, j));

	free(a);
}

#undef A
#define A(i, j) (a + 6 * (i))[j]

void
proc_reg_print_anova_table(void)
{
	int i, j;
	char **a, buf[100];

	a = (char **) xmalloc(4 * 6 * sizeof (char *));

	// 1st row

	A(0, 0) = strdup("");
	A(0, 1) = strdup("DF");
	A(0, 2) = strdup("Sum of Squares");
	A(0, 3) = strdup("Mean Square");
	A(0, 4) = strdup("F Value");
	A(0, 5) = strdup("Pr > F");

	// 2nd row

	A(1, 0) = strdup("Model");

	snprintf(buf, sizeof buf, "%d", (int) dfm);
	A(1, 1) = strdup(buf);

	snprintf(buf, sizeof buf, "%0.5f", ssr);
	A(1, 2) = strdup(buf);

	snprintf(buf, sizeof buf, "%0.5f", msr);
	A(1, 3) = strdup(buf);

	snprintf(buf, sizeof buf, "%0.2f", fval);
	A(1, 4) = strdup(buf);

	snprintf(buf, sizeof buf, "%0.4f", pval);
	A(1, 5) = strdup(buf);

	// 3rd row

	A(2, 0) = strdup("Error");

	snprintf(buf, sizeof buf, "%d", (int) dfe);
	A(2, 1) = strdup(buf);

	snprintf(buf, sizeof buf, "%0.5f", sse);
	A(2, 2) = strdup(buf);

	snprintf(buf, sizeof buf, "%0.5f", mse);
	A(2, 3) = strdup(buf);

	A(2, 4) = strdup("");
	A(2, 5) = strdup("");

	// 4th row

	A(3, 0) = strdup("Total");

	snprintf(buf, sizeof buf, "%d", (int) dft);
	A(3, 1) = strdup(buf);

	snprintf(buf, sizeof buf, "%0.5f", sst);
	A(3, 2) = strdup(buf);

	A(3, 3) = strdup("");
	A(3, 4) = strdup("");
	A(3, 5) = strdup("");

	buf[0] = 1; // left justify
	buf[1] = 0;
	buf[2] = 0;
	buf[3] = 0;
	buf[4] = 0;
	buf[5] = 0;

	print_table(a, 4, 6, buf);

	for (i = 0; i < 4; i++)
		for (j = 0; j < 6; j++)
			free(A(i, j));

	free(a);
}

void
proc_reg_print_diag_table(void)
{
	char *a[3][4], buf[100];

	a[0][0] = "Root MSE";
	a[1][0] = "Dependent Mean";
	a[2][0] = "Coeff Var";

	a[0][2] = "R-Square";
	a[1][2] = "Adj R-Sq";
	a[2][2] = "";

	snprintf(buf, sizeof buf, "%0.5f", rootmse);
	a[0][1] = strdup(buf);

	snprintf(buf, sizeof buf, "%0.5f", ybar);
	a[1][1] = strdup(buf);

	snprintf(buf, sizeof buf, "%0.5f", cv);
	a[2][1] = strdup(buf);

	snprintf(buf, sizeof buf, "%0.4f", rsquare);
	a[0][3] = strdup(buf);

	snprintf(buf, sizeof buf, "%0.4f", adjrsq);
	a[1][3] = strdup(buf);

	a[2][3] = "";

	buf[0] = 1; // left justify
	buf[1] = 0;
	buf[2] = 1;
	buf[3] = 0;

	print_table(&a[0][0], 3, 4, buf);

	free(a[0][1]);
	free(a[1][1]);
	free(a[2][1]);

	free(a[0][3]);
	free(a[1][3]);
}

#undef C
#undef G
#undef T
#undef X
#undef A
void
proc_step(void)
{
	nby = 0;
	nvar = 0;
	nstats = 0;
	nclass = 0;

	maxdec = 3;
	alpha = 0.05;

	select_dataset(NULL);

	scan(); // get token after PROC

	keyword();

	switch (token) {
	case KANOVA:
		proc_anova();
		break;
	case KMEANS:
		proc_means();
		break;
	case KPRINT:
		proc_print();
		break;
	case KREG:
		proc_reg();
		break;
	default:
		expected("procedure name");
	}
}
char *
read_text_file(char *filename)
{
	int fd, n;
	char *buf;

	fd = open(filename, O_RDONLY);

	if (fd == -1) {
		fprintf(stderr, "cannot open %s\n", filename);
		exit(1);
	}

	n = lseek(fd, 0, SEEK_END);

	if (n < 0)
		exit(1);

	if (lseek(fd, 0, SEEK_SET))
		exit(1);

	buf = malloc(n + 1);

	if (buf == NULL)
		exit(1);

	if (read(fd, buf, n) != n)
		exit(1);

	close(fd);

	buf[n] = '\0';

	return buf;
}
jmp_buf jmpbuf;

void
run(char *s)
{
	if (s == NULL)
		return;

	emit_line_init();

	run_nib(s);

	// clean up (always gets here, even on stop)

	if (infile) {
		fclose(infile);
		infile = NULL;
	}

	free_datasets();

	FREE(title)
	FREE(title1)
	FREE(title2)
	FREE(title3)
}

void
run_nib(char *s)
{
	if (setjmp(jmpbuf))
		return;

	pgm = s;

	inp = s;

	scan();

	for (;;) {
		keyword();
		switch (token) {
		case 0:
			return;
		case KDATA:
			data_step();
			break;
		case KPROC:
			proc_step();
			break;
		case KRUN:
			scan();
			if (token != ';')
				stop("';' expected");
			scan();
			break;
		default:
			parse_default();
			break;
		}
	}
}

void
parse_default(void)
{
	switch (token) {
	case ';':
		scan();
		break;
	case '*':
		parse_comment_stmt();
		break;
	case KTITLE:
		title_stmt();
		break;
	case KTITLE1:
		title1_stmt();
		break;
	case KTITLE2:
		title2_stmt();
		break;
	case KTITLE3:
		title3_stmt();
		break;
	default:
		stop("Unexpected token, did you mean proc?");
	}
}

void
parse_alpha_option(void)
{
	scan(); // skip alpha token

	if (token != '=')
		expected("equals sign");

	scan(); // skip equals sign

	if (token != NUMBER)
		expected("number");

	alpha = atof(strbuf);

	if (alpha < 0 || alpha > 1)
		expected("number between 0 and 1");
}

void
parse_data_option(void)
{
	scan();
	if (token != '=')
		stop("'=' expected");
	scan();
	if (token != NAME)
		stop("DATA=NAME expected");
	select_dataset(strbuf);
}

void
parse_maxdec_option(void)
{
	scan();
	if (token != '=')
		stop("'=' expected");

	scan();
	if (token != NUMBER)
		stop("Number expected");

	maxdec = (int) token_num;

	if (maxdec < 0)
		maxdec = 0;

	if (maxdec > 8)
		maxdec = 8;
}

void *
xmalloc(int size)
{
	void *p = malloc(size);
	if (p == NULL)
		exit(1);
	return p;
}

void *
xrealloc(void *p, int size)
{
	p = realloc(p, size);
	if (p == NULL)
		exit(1);
	return p;
}

void
print_pgm(void)
{
	char c, *s, *t;

	s = pgm;

	for (;;) {

		while (*s && s != inp && (*s == '\n' || *s == '\r'))
			s++;

		if (*s == 0 || s == inp)
			break;

		t = s;

		while (*s && s != inp && *s != '\n' && *s != '\r')
			s++;

		c = *s;
		*s = 0;
		emit_line(t);
		*s = c;
	}
}

void
stop(char *s)
{
	print_pgm();
	emit_line(s);
	longjmp(jmpbuf, 1);
}

void
expected(char *s)
{
	if (token == 0)
		snprintf(errbuf, ERRBUFLEN, "Expected %s before end of program", s);
	else if (token == ';')
		snprintf(errbuf, ERRBUFLEN, "Expected %s before end of statement", s);
	else
		snprintf(errbuf, ERRBUFLEN, "Expected %s instead of \"%s\"", s, strbuf);
	stop(errbuf);
}
void
scan(void)
{
	static int t;

	if (inp == pgm)
		t = 0; // initial state
	else
		t = token;

	token = scan1();

	// ensure last token is semicolon

	if (token == 0 && t && t != ';') {
		strbuf[0] = ';';
		strbuf[1] = 0;
		token = ';';
	}
}

int
scan1(void)
{
	int i, n;

	// skip spaces

	while (*inp && *inp <= ' ')
		inp++;

	token_str = inp;

	// end?

	if (*inp == '\0') {
		strbuf[0] = 0;
		return 0;
	}

	// name?

	if (isalpha(*inp) || *inp == '_') {
		for (i = 0; i < STRBUFLEN; i++) {
			if (isalnum(*inp) || *inp == '_')
				strbuf[i] = toupper(*inp++);
			else
				break;
		}
		if (i == STRBUFLEN)
			stop("Name too long");
		strbuf[i] = 0;
		return NAME;
	}

	// number?

	if (isdigit(*inp)
	|| (inp[0] == '.' && isdigit(inp[1]))
	|| ((*inp == '+' || *inp == '-')
	&& (isdigit(inp[1]) || (inp[1] == '.' && isdigit(inp[2]))))) {
		if (*inp == '+' || *inp == '-')
			inp++;
		while (isdigit(*inp))
			inp++;
		if (*inp == '.') {
			inp++;
			while (isdigit(*inp))
				inp++;
		}
		if (*inp == 'E' || *inp == 'e') {
			inp++;
			if (*inp == '+' || *inp == '-')
				inp++;
			while (isdigit(*inp))
				inp++;
		}
		n = (int) (inp - token_str);
		if (n >= STRBUFLEN)
			stop("Number format?");
		for (i = 0; i < n; i++)
			strbuf[i] = token_str[i];
		strbuf[i] = 0;
		n = sscanf(strbuf, "%lg", &token_num);
		if (n < 1)
			stop("Number format?");
		return NUMBER;
	}

	// string?

	if (*inp == '\'') {
		inp++;
		for (i = 0; i < STRBUFLEN; i++) {
			if (*inp == 0)
				break;
			if (*inp == '\r' || *inp == '\n')
				stop("Runaway string");
			if (inp[0] == '\'' && inp[1] == '\'') {
				strbuf[i] = *inp;
				inp += 2;
				continue;
			}
			if (*inp == '\'') {
				inp++;
				break;
			}
			strbuf[i] = *inp++;
		}
		if (i == STRBUFLEN)
			stop("String too long");
		strbuf[i] = 0;
		return STRING;
	}

	if (*inp == '"') {
		inp++;
		for (i = 0; i < STRBUFLEN; i++) {
			if (*inp == 0)
				break;
			if (*inp == '\r' || *inp == '\n')
				stop("Runaway string");
			if (inp[0] == '"' && inp[1] == '"') {
				strbuf[i] = *inp;
				inp += 2;
				continue;
			}
			if (*inp == '"') {
				inp++;
				break;
			}
			strbuf[i] = *inp++;
		}
		if (i == STRBUFLEN)
			stop("String too long");
		strbuf[i] = 0;
		return STRING;
	}

	// double at sign?

	if (inp[0] == '@' && inp[1] == '@') {
		strcpy(strbuf, "@@");
		inp += 2;
		return ATAT;
	}

	// double asterisk?

	if (inp[0] == '*' && inp[1] == '*') {
		strcpy(strbuf, "**");
		inp += 2;
		return STARSTAR;
	}

	// anything else is a single character token

	strbuf[0] = *inp;
	strbuf[1] = 0;

	return *inp++;
}

struct key {
	char *s;
	int k;
} keytab[] = {
	{"ALPHA",	KALPHA},
	{"ANOVA",	KANOVA},
	{"BY",		KBY},
	{"CARDS",	KCARDS},
	{"CLASS",	KCLASS},
	{"CLM",		KCLM},
	{"DATA",	KDATA},
	{"DATALINES",	KDATALINES},
	{"DELIMITER",	KDELIMITER},
	{"DLM",		KDLM},
	{"FIRSTOBS",	KFIRSTOBS},
	{"INFILE",	KINFILE},
	{"INPUT",	KINPUT},
	{"LCLM",	KLCLM},
	{"LSD",		KLSD},
	{"MAX",		KMAX},
	{"MAXDEC",	KMAXDEC},
	{"MEAN",	KMEAN},
	{"MEANS",	KMEANS},
	{"MIN",		KMIN},
	{"MODEL",	KMODEL},
	{"N",		KN},
	{"NOINT",	KNOINT},
	{"PRINT",	KPRINT},
	{"PROC",	KPROC},
	{"RANGE",	KRANGE},
	{"REG",		KREG},
	{"RUN",		KRUN},
	{"STD",		KSTD},
	{"STDDEV",	KSTDDEV},
	{"STDERR",	KSTDERR},
	{"STDMEAN",	KSTDMEAN},
	{"SUM",		KSUM},
	{"T",		KT},
	{"TITLE",	KTITLE},
	{"TITLE1",	KTITLE1},
	{"TITLE2",	KTITLE2},
	{"TITLE3",	KTITLE3},
	{"TTEST",	KTTEST},
	{"UCLM",	KUCLM},
	{"VAR",		KVAR},
	{"WELCH",	KWELCH},
};

void
keyword(void)
{
	int i, n = sizeof keytab / sizeof (struct key);
	if (token != NAME)
		return;
	for (i = 0; i < n; i++)
		if (strcmp(strbuf, keytab[i].s) == 0) {
			token = keytab[i].k;
			break;
		}
}

// get data lines following DATALINES

char *
get_dataline(char *buf, int len)
{
	int i;

	// scan to end of current line

	while (*inp && *inp != ';' && *inp != '\n' && *inp != '\r')
		inp++;

	// end of input?

	if (*inp == 0 || *inp == ';')
		return NULL;

	// skip end of line

	if (inp[0] == '\r' && inp[1] == '\n')
		inp += 2;
	else
		inp += 1;

	while (isspace(*inp) && *inp != '\n' && *inp != '\r')
		inp++;

	if (*inp == 0 || *inp == ';' || *inp == '\n' || *inp == '\r')
		return NULL;

	// copy data line into buf

	for (i = 0; i < len - 1; i++) {
		buf[i] = *inp++;
		if (*inp == 0 || *inp == ';' || *inp == '\n' || *inp == '\r')
			break;
	}

	if (i == len - 1)
		stop("Buffer overrun");

	buf[i + 1] = 0;

	return buf;
}

void
get_next_token(void)
{
	scan();
	if (token == NAME)
		keyword();
}
// t-distribution cdf, like pt() in R, used for computing p-values

double
tdist(double t, double df)
{
	double x;
	if (isnan(t) || !isfinite(df) || df <= 0.0)
		return NAN;
	if (t == INFINITY)
		return 1.0; // pt(Inf,1) == 1
	if (t == -INFINITY)
		return 0.0; // pt(-Inf,1) == 0
	x = (t + sqrt(t * t + df)) / (2.0 * sqrt(t * t + df));
	return incbeta(df / 2.0, df / 2.0, x);
}

// t-distribution quantile function, like qt() in R

// used for computing confidence intervals

double
qt(double p, double df)
{
	int i;
	double a, t, t1, t2;
	if (isnan(p) || p < 0.0 || p > 1.0 || !isfinite(df) || df <= 0.0)
		return NAN;
	if (p == 0.0)
		return -INFINITY; // qt(0,1) == -Inf
	if (p == 1.0)
		return INFINITY; // qt(1,1) == Inf
	t1 = -1e5;
	t2 = 1e5;
	for (i = 0; i < 100; i++) {
		t = 0.5 * (t1 + t2);
		a = tdist(t, df);
		if (fabs(a - p) < 1e-9)
			break;
		if (a < p)
			t1 = t;
		else
			t2 = t;
	}
	return t;
}

// F-distribution cdf, like pf() in R

double
fdist(double t, double df1, double df2)
{
	if (isnan(t) || !isfinite(df1) || !isfinite(df2) || df1 <= 0.0 || df2 <= 0.0)
		return NAN;
	if (t == INFINITY)
		return 1.0; // pf(Inf,1,1) == 1
	if (t <= 0.0)
		return 0.0; // pf(0,1,1) == 0
	return incbeta(df1 / 2.0, df2 / 2.0, t / (t + df2 / df1));
}
void
title_stmt(void)
{
	scan();
	if (token != STRING)
		expected("string");
	if (title)
		free(title);
	if (strbuf[0] == 0)
		title = NULL;
	else
		title = strdup(strbuf);
	scan();
	if (token != ';')
		stop("';' expected");
	scan();
}

void
title1_stmt(void)
{
	scan();
	if (token != STRING)
		expected("string");
	if (title1)
		free(title1);
	if (strbuf[0] == 0)
		title1 = NULL;
	else
		title1 = strdup(strbuf);
	scan();
	if (token != ';')
		stop("';' expected");
	scan();
}

void
title2_stmt(void)
{
	scan();
	if (token != STRING)
		expected("string");
	if (title2)
		free(title2);
	if (strbuf[0] == 0)
		title2 = NULL;
	else
		title2 = strdup(strbuf);
	scan();
	if (token != ';')
		stop("';' expected");
	scan();
}

void
title3_stmt(void)
{
	scan();
	if (token != STRING)
		expected("string");
	if (title3)
		free(title3);
	if (strbuf[0] == 0)
		title3 = NULL;
	else
		title3 = strdup(strbuf);
	scan();
	if (token != ';')
		stop("';' expected");
	scan();
}
