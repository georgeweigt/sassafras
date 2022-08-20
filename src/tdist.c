#include "defs.h"

// t-distribution cdf, like pt() in R

// used for computing p-values

double
tdist(double t, double df)
{
	double x;
	if (!isfinite(t) || !isfinite(df) || df < 1.0)
		return NAN;
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
	if (!isfinite(p) || !isfinite(df) || df < 1.0)
		return NAN;
	t1 = -1000.0;
	t2 = 1000.0;
	for (i = 0; i < 100; i++) {
		t = 0.5 * (t1 + t2);
		a = tdist(t, df);
		if (fabs(a - p) < 1e-10)
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
	if (!isfinite(t) || !isfinite(df1) || !isfinite(df2) || df1 < 1.0 || df2 < 1.0)
		return NAN;
	if (t < 0.0)
		return 0.0;
	return incbeta(df1 / 2.0, df2 / 2.0, t / (t + df2 / df1));
}
