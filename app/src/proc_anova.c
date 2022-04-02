#include "defs.h"

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

static double *B;
static int df[MAXVAR];
static double *GG;
static int kk[MAXVAR];
int *miss;
static int ncol;
static int nobs;
static int npar;
static int nx;
static double *TT;
static double *XX;
static int xx[MAXVAR];
static int xtab[MAXVAR];
static double *Y;
static double ybar;
static int yvar;
static int gstate;
static double *Yhat;
static double css;
static double ssr;
static double sse;
static double ss[MAXVAR];
static double msr;
static double mse;
static double rootmse;
static double fval;
static double pval;
static double rsquare;
static double adjrsq;
static double cv;
static char buf[1000];
static int nmiss;
#define MAXLVL 100
static double mean[MAXLVL];
static double variance[MAXLVL];
static int count[MAXLVL];
#define MAXITEM 100
static int item[MAXITEM];
static double dfe;

#define G(i, j) (GG + (i) * ncol)[j]
#define T(i, j) (TT + (i) * ncol)[j]
#define X(i, j) (XX + (i) * ncol)[j]

static void parse_proc_anova_stmt(void);
static void parse_proc_anova_body(void);
static void parse_class_stmt(void);
static void parse_model_stmt(void);
static void parse_model_options(void);
static void parse_explanatory_variable(void);
static void regression(void);
static int get_var_index(void);
static void add_interaction(int);
static void create_interaction_level_names(int);
static void add_factorial(int);
static void add_nested(int);
static void prelim(void);
static int compute_G(void);
static void fit(void);
static void fit1(int, int);
static void compute_B(void);
static void compute_Yhat(void);
static void compute_ss(void);
static void print_anova_table_part1(void);
static void print_anova_table_part2(void);
static void print_anova_table_part3(void);
static void model(void);
static void compute_means(int x);
static void print_means(int x);
static void parse_means_stmt(void);
static void parse_means_item(void);
static void print_lsd(int);
static void print_ttest(int);

void
proc_anova()
{
	nx = 0;

	parse_proc_anova_stmt();

	if (dataset == NULL)
		stop("No data set");

	parse_proc_anova_body();
}

static void
parse_proc_anova_stmt()
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

static void
parse_proc_anova_body()
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
			parse_means_stmt();
			break;
		case KMODEL:
			parse_model_stmt();
			model();
			break;
		default:
			parse_default();
			break;
		}
	}
}

static void
parse_class_stmt()
{
}

static void
parse_means_stmt()
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

		parse_means_item();

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
		compute_means(x);
		print_means(x);
		if (lsd)
			print_lsd(x);
		if (ttest)
			print_ttest(x);
	}
}

// An item is a variable name or an interaction

// Returns token in buf[]

static void
parse_means_item()
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

static void
parse_model_stmt()
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

		parse_explanatory_variable();
	}

	if (token == '/')
		parse_model_options();

	scan(); // eat the semicolon
}

static void
parse_model_options()
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

static void
parse_explanatory_variable()
{
	int n = 1;

	xx[0] = get_var_index();

	scan();

	switch (token) {

	case '*': // name * name ...

		do {
			scan();
			if (token != NAME)
				expected("variable name");
			if (n == MAXVAR)
				stop("Too many interaction terms");
			xx[n++] = get_var_index();
			scan();
		} while (token == '*');
		add_interaction(n);
		break;

	case '|': // name | name ...

		do {
			scan();
			if (token != NAME)
				expected("variable name");
			if (n == MAXVAR)
				stop("Too many interaction terms");
			xx[n++] = get_var_index();
			scan();
		} while (token == '|');
		add_factorial(n);
		break;

	case '(': // name(name name ...)

		scan();
		while (token == NAME) {
			if (n == MAXVAR)
				stop("Too many interaction terms");
			xx[n++] = get_var_index();
			scan();
		};
		if (token != ')')
			expected("right parenthesis ')'");
		scan();
		add_nested(n);
		break;

	default:
		if (nx == MAXVAR)
			stop("Too many explanatory variables");
		xtab[nx++] = xx[0];
		break;
	}
}

static int
get_var_index()
{
	int i, n;
	n = dataset->nvar;
	for (i = 0; i < n; i++)
		if (strcmp(dataset->spec[i].name, strbuf) == 0)
			break;
	if (i == n) {
		sprintf(buf, "The variable name \"%s\" is not in the data set", strbuf);
		stop(buf);
	}
	if (dataset->spec[i].ltab == NULL) {
		sprintf(buf, "The variable \"%s\" is numeric, please change to categorical in the data step", strbuf);
		stop(buf);
	}
	return i;
}

// There are n interaction names in xx[]

static void
add_interaction(int n)
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

		create_interaction_level_names(n);

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

static void
create_interaction_level_names(int n)
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

static void
add_factorial(int n)
{
}

static void
add_nested(int n)
{
}

static void
model()
{
	regression();

	print_title();

	if (nmiss) {
		sprintf(buf, "%d observations deleted due to missing data", nmiss);
		emit_line(buf);
	}

	emit_line_center("Analysis of Variance");
	emit_line("");

	print_anova_table_part1();

	print_anova_table_part2();

	print_anova_table_part3();
}

static void
regression()
{
	prelim();

	fit();

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

static void
prelim()
{
	int i, j, k, x;

	if (miss)
		free(miss);

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

	if (B) {
		free(B);
		free(GG);
		free(TT);
		free(XX);
		free(Y);
		free(Yhat);
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

static void
fit()
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
			fit1(x, j); // fit next column
		df[i] = npar;
		if (gstate == -1)
			compute_G();
		compute_B();
		compute_Yhat();
		compute_ss();
		ss[i] = ssr;
	}

	// differentials

	for (i = nx - 1; i > 0; i--) {
		df[i] -= df[i - 1];
		ss[i] -= ss[i - 1];
	}

	df[0]--; // subtract 1 for intercept
}

static void
fit1(int x, int level)
{
	int i, k = 0;

	for (i = 0; i < dataset->nobs; i++) {
		if (miss[i])
			continue;
		X(k++, npar) = (dataset->spec[x].v[i] == level) ? 1 : 0;
	}

	npar++;

	gstate = compute_G();

	if (gstate == -1)
		npar--; // X'X is singular hence remove column
}

static int
compute_G()
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

static void
compute_B()
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

static void
compute_Yhat()
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

static void
compute_ss()
{
	int i;

	ssr = 0;
	sse = 0;

	for (i = 0; i < nobs; i++) {
		ssr += (Yhat[i] - ybar) * (Yhat[i] - ybar);
		sse += (Y[i] - Yhat[i]) * (Y[i] - Yhat[i]);
	}
}

#if 0

static void
print_T()
{
	int i, j;
	printf("T =\n");
	for (i = 0; i < npar; i++) {
		for (j = 0; j < npar; j++)
			printf(" %g", X(i, j));
		printf("\n");
	}
}

static void
print_X()
{
	int i, j;
	printf("X =\n");
	for (i = 0; i < nobs; i++) {
		for (j = 0; j < npar; j++)
			printf(" %g", X(i, j));
		printf("\n");
	}
}

#endif

#undef A
#define A(i, j) (a + 6 * (i))[j]

void
print_anova_table_part1()
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

	sprintf(buf, "%d", npar - 1);
	A(1, 1) = strdup(buf);

	sprintf(buf, "%0.8f", ssr);
	A(1, 2) = strdup(buf);

	sprintf(buf, "%0.8f", msr);
	A(1, 3) = strdup(buf);

	sprintf(buf, "%0.2f", fval);
	A(1, 4) = strdup(buf);

	sprintf(buf, "%0.4f", pval);
	A(1, 5) = strdup(buf);

	// 3rd row

	A(2, 0) = strdup("Error");

	sprintf(buf, "%d", nobs - npar);
	A(2, 1) = strdup(buf);

	sprintf(buf, "%0.8f", sse);
	A(2, 2) = strdup(buf);

	sprintf(buf, "%0.8f", mse);
	A(2, 3) = strdup(buf);

	A(2, 4) = strdup("");
	A(2, 5) = strdup("");

	// 4th row

	A(3, 0) = strdup("Total");

	sprintf(buf, "%d", nobs - 1);
	A(3, 1) = strdup(buf);

	sprintf(buf, "%0.8f", css);
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

static void
print_anova_table_part2()
{
	char **a;

	a = (char **) xmalloc(8 * sizeof (char *));

	A(0, 0) = strdup("R-Square");
	A(0, 1) = strdup("Coeff Var");
	A(0, 2) = strdup("Root MSE");

	sprintf(buf, "%s Mean", dataset->spec[yvar].name);
	A(0, 3) = strdup(buf);

	sprintf(buf, "%0.6f", rsquare);
	A(1, 0) = strdup(buf);

	sprintf(buf, "%0.6f", cv);
	A(1, 1) = strdup(buf);

	sprintf(buf, "%0.6f", rootmse);
	A(1, 2) = strdup(buf);

	sprintf(buf, "%0.6f", ybar);
	A(1, 3) = strdup(buf);

	buf[0] = 0; // right justified
	buf[1] = 0;
	buf[2] = 0;
	buf[3] = 0;

	print_table_and_free(a, 2, 4, buf);
}

#undef A
#define A(i, j) (a + 6 * (i))[j]

static void
print_anova_table_part3()
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

		sprintf(buf, "%d", df[i]);
		A(i + 1, 1) = strdup(buf);

		if (df[i] == 0) {
			A(i + 1, 2) = strdup(".");
			A(i + 1, 3) = strdup(".");
			A(i + 1, 4) = strdup(".");
			A(i + 1, 5) = strdup(".");
			continue;
		}

		// Anova SS

		sprintf(buf, "%0.8f", ss[i]);
		A(i + 1, 2) = strdup(buf);

		// Mean Square

		msq = ss[i] / df[i];

		sprintf(buf, "%0.8f", msq);
		A(i + 1, 3) = strdup(buf);

		// F Value

		fval = msq / mse;

		sprintf(buf, "%0.2f", fval);
		A(i + 1, 4) = strdup(buf);

		// Pr > F

		pval = 1 - fdist(fval, df[i], nobs - npar);

		sprintf(buf, "%0.4f", pval);
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

static void
compute_means(int x)
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

static void
print_means(int x)
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
	sprintf(buf, "Mean %s", s);
	A(0, m + 2) = strdup(buf);

	sprintf(buf, "%g%% CI MIN", 100 * (1 - alpha));
	A(0, m + 3) = strdup(buf);

	sprintf(buf, "%g%% CI MAX", 100 * (1 - alpha));
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

		sprintf(buf, "%d", count[i]);
		A(i + 1, m + 1) = strdup(buf);

		if (count[i] < 1) {
			A(i + 1, m + 2) = strdup(".");
			A(i + 1, m + 3) = strdup(".");
			A(i + 1, m + 4) = strdup(".");
			continue;
		}

		// Mean

		sprintf(buf, "%0.6f", mean[i]);
		A(i + 1, m + 2) = strdup(buf);

		// Confidence Interval

		t = q * sqrt(mse / count[i]);

		sprintf(buf, "%0.6f", mean[i] - t);
		A(i + 1, m + 3) = strdup(buf);

		sprintf(buf, "%0.6f", mean[i] + t);
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

static void
print_lsd(int x)
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
	sprintf(buf, "Delta %s", s);
	A(0, 2) = strdup(buf);

	sprintf(buf, "%g%% CI MIN", 100 * (1 - alpha));
	A(0, 3) = strdup(buf);

	sprintf(buf, "%g%% CI MAX", 100 * (1 - alpha));
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
			sprintf(buf, "%0.6f", d);
			A(k, 2) = strdup(buf);

			// confidence interval

			se = sqrt(mse * (1.0 / count[i] + 1.0 / count[j]));

			lsd = q * se;

			tval = d / se;

			pval = 2 * (1 - tdist(fabs(tval), dfe));

			sprintf(buf, "%0.6f", d - lsd);
			A(k, 3) = strdup(buf);

			sprintf(buf, "%0.6f", d + lsd);
			A(k, 4) = strdup(buf);

			// t Value

			sprintf(buf, "%0.2f", tval);
			A(k, 5) = strdup(buf);

			// Pr > |t|

			if (pval > alpha)
				sprintf(buf, "%0.4f  ", pval);
			else
				sprintf(buf, "%0.4f *", pval);
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

static void
print_ttest(int x)
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
	sprintf(buf, "Delta %s", s);
	A(0, 2) = strdup(buf);

	sprintf(buf, "%g%% CI MIN", 100 * (1 - alpha));
	A(0, 3) = strdup(buf);

	sprintf(buf, "%g%% CI MAX", 100 * (1 - alpha));
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
			sprintf(buf, "%0.6f", d);
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

			sprintf(buf, "%0.6f", d - t);
			A(k, 3) = strdup(buf);

			sprintf(buf, "%0.6f", d + t);
			A(k, 4) = strdup(buf);

			sprintf(buf, "%0.2f", tval);
			A(k, 5) = strdup(buf);

			if (pval > alpha)
				sprintf(buf, "%0.4f  ", pval);
			else
				sprintf(buf, "%0.4f *", pval);
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
