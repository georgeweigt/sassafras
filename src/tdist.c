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
qt(double x, double df)
{
	int i;
	double a, b, c, y;
	if (!isfinite(x) || !isfinite(df))
		return NAN;
	if (x < 1e-12)
		return -INFINITY;
	if (x == 0.5)
		return 0.0;
	if (x > 1.0 - 1e-12)
		return INFINITY;
	a = -100.0;
	b = 100.0;
	for (i = 0; i < 50; i++) {
		c = 0.5 * (a + b);
		y = tdist(c, df);
		if (!isfinite(y))
			return NAN;
		if (y < x)
			a = c;
		else
			b = c;
	}
	return c;
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
