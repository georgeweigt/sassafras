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
