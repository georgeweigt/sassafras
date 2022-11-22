#include "defs.h"

static char **a;
static char tblfmt[MAXVAR + MAXSTAT + 1];
static int row, nrow, ncol;
static int filter[MAXVAR];

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
		filter[k] = i;
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
				level = filter[j];
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

static char *fmt[9] = {
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
			if (dataset->spec[k].v[i] != filter[j])
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
