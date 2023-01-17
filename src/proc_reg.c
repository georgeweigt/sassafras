//	ncol	Number explanatory variables (including intercept)
//
//	nrow	Number of observations
//
//
//	B [ncol]	Regression coefficients
//
//	C [ncol][ncol]	Coefficient matrix
//
//	G [ncol][ncol]	Inverse of X'X
//
//	T [ncol][ncol]	Temporary matrix
//
//	X [nrow][ncol]	Design matrix
//
//	Y [nrow]	Response vector
//
//	Yhat [nrow]	Predicted response X * B
//
//	Z [ncol]	Zapped columns to make X'X nonsingular
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
//	sse	Sum of squares error
//
//	ssr	Sum of squares regression
//
//	sst	Sum of squares total
//
//	ybar	Mean of response variable Y
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

// Yhat = X * B

void
proc_reg_compute_Yhat(void)
{
	int i, j, k;
	double yhat;
	for (i = 0; i < nrow; i++) {
		yhat = 0;
		k = 0;
		for (j = 0; j < ncol; j++)
			if (Z[j] == 0) // if not zapped
				yhat += X(i, j) * B[k++];
		Yhat[i] = yhat;
	}
}

void
proc_reg_compute_mse(void)
{
	int i;
	double d;

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

		d = Yhat[i] - ybar;
		ssr += d * d;

		d = Y[i] - Yhat[i];
		sse += d * d;

		d = Y[i] - ybar;
		sst += d * d;
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

	xfree(Z);
	xfree(Y);
	xfree(Yhat);
	xfree(B);
	xfree(SE);
	xfree(TVAL);
	xfree(PVAL);
	xfree(CC);
	xfree(GG);
	xfree(TT);
	xfree(XX);

	Z = xmalloc(ncol * sizeof (int));

	Y = xmalloc(nrow * sizeof (double));
	Yhat = xmalloc(nrow * sizeof (double));
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

	proc_reg_compute_Yhat();

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
