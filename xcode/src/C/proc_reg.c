#include "defs.h"

static int noint;

// explanatory variables from the MODEL statement
static int num_x;
static int xtab[MAXVAR];

// response variables from the MODEL statement
static int num_y;
static int ytab[MAXVAR];

static void parse_proc_reg_stmt(void);
static void parse_proc_reg_body(void);
static void parse_model_stmt(void);
static void parse_model_options(void);

void
proc_reg(void)
{
	num_x = 0;
	num_y = 0;

	parse_proc_reg_stmt();

	if (dataset == NULL)
		stop("No data set");

	parse_proc_reg_body();

	regress();

	print_title();

	emit_line_center("Analysis of Variance");
	emit_line("");

	print_anova_table();
	print_diag_table();

	emit_line_center("Parameter Estimates");
	emit_line("");

	print_parameter_estimates();
}

static void
parse_proc_reg_stmt(void)
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

static void
parse_proc_reg_body(void)
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
			parse_model_stmt();
			break;
		default:
			parse_default();
			break;
		}
	}
}

static void
parse_model_stmt(void)
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
		sprintf(errbuf, "Variable %s not in dataset", strbuf);
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
			sprintf(errbuf, "Variable %s not in dataset", strbuf);
			stop(errbuf);
		}

		xtab[num_x++] = i;
	}

	if (token == '/')
		parse_model_options();

	scan(); // eat the semicolon
}

static void
parse_model_options(void)
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

static int nrow;
static int ncol;

static int npar;

static double ybar;

static double ssr;
static double sse;
static double sst; 

static double msr;
static double mse;

static double fval;
static double pval;

static double rsquare; // aka coefficient of determination
static double adjrsq; // see KNNL p. 226
static double rootmse;
static double cv;
static int dfm; // degrees of freedom model
static int dfe; // degrees of freedom error
static int dft; // degrees of freedom total

static int *Z;

static double *B;
static double *Y;
static double *SE;
static double *TVAL;
static double *PVAL;

static double *_C_;
static double *_G_;
static double *_T_;
static double *_X_;

#define C(i, j) (_C_ + (i) * ncol)[j]
#define G(i, j) (_G_ + (i) * ncol)[j]
#define T(i, j) (_T_ + (i) * ncol)[j]
#define X(i, j) (_X_ + (i) * ncol)[j]

// X is the design matrix

void
compute_X(void)
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
compute_T(void)
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
compute_G(void)
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

static void
compute_B(void)
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
compute_mse(void)
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

	pval = 1 - fdist(fval, dfm, dfe);

	rsquare = 1 - sse / sst;

	adjrsq = 1 - (double) dft / dfe * sse / sst;

	cv = 100 * rootmse / ybar;
}

// C = mse * G

void
compute_C(void)
{
	int i, j;
	for (i = 0; i < npar; i++)
		for (j = 0; j < npar; j++)
			C(i, j) = mse * G(i, j);
}

// SE[i] = sqrt(C[i][i])

void
compute_SE(void)
{
	int i;
	for (i = 0; i < npar; i++)
		SE[i] = sqrt(C(i, i));
}

void
compute_TVAL(void)
{
	int i;
	for (i = 0; i < npar; i++)
		TVAL[i] = B[i] / SE[i];
}

void
compute_PVAL(void)
{
	int i, n;
	n = nrow - npar;
	for (i = 0; i < npar; i++)
		PVAL[i] = 2 * (1 - tdist(fabs(TVAL[i]), n));
}

void
print_B(void)
{
	int i;
	printf("B =\n");
	for (i = 0; i < npar; i++)
		printf("%20g\n", B[i]);
}

void
print_X(void)
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
print_T(void)
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
print_G(void)
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
print_Z(void)
{
	int i;
	printf("Z =\n");
	for (i = 0; i < ncol; i++)
		printf("%20d\n", Z[i]);
}

void
regress(void)
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

	if (Z) {
		free(Z);
		free(Y);
		free(B);
		free(SE);
		free(TVAL);
		free(PVAL);
		free(_C_);
		free(_G_);
		free(_T_);
		free(_X_);
	}

	Z = xmalloc(ncol * sizeof (int));

	Y = xmalloc(nrow * sizeof (double));
	B = xmalloc(ncol * sizeof (double));
	SE = xmalloc(ncol * sizeof (double));
	TVAL = xmalloc(ncol * sizeof (double));
	PVAL = xmalloc(ncol * sizeof (double));

	_C_ = xmalloc(ncol * ncol * sizeof (double));
	_G_ = xmalloc(ncol * ncol * sizeof (double));
	_T_ = xmalloc(ncol * ncol * sizeof (double));
	_X_ = xmalloc(nrow * ncol * sizeof (double));

	compute_X();

	npar = ncol;

	for (i = 0; i < ncol; i++)
		Z[i] = 0;

	compute_T();

	// if singular then put in columns one by one

	if (compute_G() == -1) {
		npar = 0;
		for (i = 0; i < ncol; i++)
			Z[i] = 1;
		for (i = 0; i < ncol; i++) {
			npar++;
			Z[i] = 0;
			compute_T();
			if (compute_G() == -1) {
				npar--;
				Z[i] = 1;
			}
		}

		// did last column get zapped?

		if (Z[ncol - 1]) {
			compute_T();
			compute_G();
		}
	}

	// sanity check

	if (npar < 1 || npar >= nrow) {
		sprintf(errbuf, "Regression model kaput, p = %d, n = %d, must have 0 < p < n", npar, nrow);
		stop(errbuf);
	}

	compute_B();

	compute_mse();

	compute_C();

	compute_SE();

	compute_TVAL();

	compute_PVAL();
}

#define A(i, j) (a + 5 * (i))[j]

void
print_parameter_estimates(void)
{
	int i, j, k, m, n, x;
	static char s[100];

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
				sprintf(s, "%s %s", dataset->spec[x].name, dataset->spec[x].ltab[j]); // FIXME check for buffer overrun
				A(k++, 0) = strdup(s);
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

		sprintf(s, "%0.5f", B[k]);
		A(i, 1) = strdup(s);

		sprintf(s, "%0.5f", SE[k]);
		A(i, 2) = strdup(s);

		sprintf(s, "%0.2f", TVAL[k]);
		A(i, 3) = strdup(s);

		sprintf(s, "%0.4f", PVAL[k]);
		A(i, 4) = strdup(s);

		k++;
	}

	s[0] = 1; // right justify
	s[1] = 0;
	s[2] = 0;
	s[3] = 0;
	s[4] = 0;

	print_table(a, m, 5, s);

	for (i = 0; i < m; i++)
		for (j = 0; j < 5; j++)
			free(A(i, j));

	free(a);
}

#undef A
#define A(i, j) (a + 6 * (i))[j]

void
print_anova_table(void)
{
	int i, j;
	char **a, s[100];

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

	sprintf(s, "%d", dfm);
	A(1, 1) = strdup(s);

	sprintf(s, "%0.5f", ssr);
	A(1, 2) = strdup(s);

	sprintf(s, "%0.5f", msr);
	A(1, 3) = strdup(s);

	sprintf(s, "%0.2f", fval);
	A(1, 4) = strdup(s);

	sprintf(s, "%0.4f", pval);
	A(1, 5) = strdup(s);

	// 3rd row

	A(2, 0) = strdup("Error");

	sprintf(s, "%d", dfe);
	A(2, 1) = strdup(s);

	sprintf(s, "%0.5f", sse);
	A(2, 2) = strdup(s);

	sprintf(s, "%0.5f", mse);
	A(2, 3) = strdup(s);

	A(2, 4) = strdup("");
	A(2, 5) = strdup("");

	// 4th row

	A(3, 0) = strdup("Total");

	sprintf(s, "%d", dft);
	A(3, 1) = strdup(s);

	sprintf(s, "%0.5f", sst);
	A(3, 2) = strdup(s);

	A(3, 3) = strdup("");
	A(3, 4) = strdup("");
	A(3, 5) = strdup("");

	s[0] = 1; // left justify
	s[1] = 0;
	s[2] = 0;
	s[3] = 0;
	s[4] = 0;
	s[5] = 0;

	print_table(a, 4, 6, s);

	for (i = 0; i < 4; i++)
		for (j = 0; j < 6; j++)
			free(A(i, j));

	free(a);
}

#if 0

// diag table style for proc glm

void
print_diag_table(void)
{
	char *a[2][4], s[100];

	// 1st row

	a[0][0] = "R Square";
	a[0][1] = "Coeff Var";
	a[0][2] = "Root MSE";
	a[0][3] = "Y Mean";

	// 2nd row

	sprintf(s, "%0.6f", rsquare);
	a[1][0] = strdup(s);

	sprintf(s, "%0.6f", cv);
	a[1][1] = strdup(s);

	sprintf(s, "%0.6f", rootmse);
	a[1][2] = strdup(s);

	sprintf(s, "%0.6f", ybar);
	a[1][3] = strdup(s);

	s[0] = 0;
	s[1] = 0;
	s[2] = 0;
	s[3] = 0;

	print_table(&a[0][0], 2, 4, s);

	free(a[1][0]);
	free(a[1][1]);
	free(a[1][2]);
	free(a[1][3]);
}

#else

void
print_diag_table(void)
{
	char *a[3][4], s[100];

	a[0][0] = "Root MSE";
	a[1][0] = "Dependent Mean";
	a[2][0] = "Coeff Var";

	a[0][2] = "R-Square";
	a[1][2] = "Adj R-Sq";
	a[2][2] = "";

	sprintf(s, "%0.5f", rootmse);
	a[0][1] = strdup(s);

	sprintf(s, "%0.5f", ybar);
	a[1][1] = strdup(s);

	sprintf(s, "%0.5f", cv);
	a[2][1] = strdup(s);

	sprintf(s, "%0.4f", rsquare);
	a[0][3] = strdup(s);

	sprintf(s, "%0.4f", adjrsq);
	a[1][3] = strdup(s);

	a[2][3] = "";

	s[0] = 1; // left justify
	s[1] = 0;
	s[2] = 1;
	s[3] = 0;

	print_table(&a[0][0], 3, 4, s);

	free(a[0][1]);
	free(a[1][1]);
	free(a[2][1]);

	free(a[0][3]);
	free(a[1][3]);
}

#endif
