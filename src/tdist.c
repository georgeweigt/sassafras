#include "defs.h"

// t-distribution quantile function, like qt() in R

double
qt(double p, double df)
{
	int i;
	double a, t, t1, t2;
	if (isnan(p) || isnan(df) || df < 1.0)
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

// t-distribution cdf, like pt() in R

double
tdist(double t, double df)
{
	double a;
	if (isnan(t) || isnan(df) || df < 1.0)
		return NAN;
	a = 0.5 * betai(0.5 * df, 0.5, df / (df + t * t));
	if (t > 0.0)
		a = 1.0 - a;
	return a;
}

// F-distribution cdf, like pf() in R

double
fdist(double t, double df1, double df2)
{
	if (isnan(t) || isnan(df1) || isnan(df2) || df1 < 1.0 || df2 < 1.0)
		return NAN;
	if (t < 0.0)
		return 0.0;
	else
		return betai(0.5 * df1, 0.5 * df2, t / (t + df2 / df1));
}

// From Numerical Recipes in C, p. 214

double
gammln(double xx)
{
	double x,y,tmp,ser;
	static double cof[6]={76.18009172947146,-86.50532032941677,
		24.01409824083091,-1.231739572450155,
		0.1208650973866179e-2,-0.5395239384953e-5};
	int j;
	y=x=xx;
	tmp=x+5.5;
	tmp -= (x+0.5)*log(tmp);
	ser=1.000000000190015;
	for (j=0;j<=5;j++) ser += cof[j]/++y;
	return -tmp+log(2.5066282746310005*ser/x);
}

// From Numerical Recipes in C, p. 227

double
betai(double a, double b, double x)
{
	double bt;
	if (x < 0.0 || x > 1.0) return NAN;
	if (x == 0.0 || x == 1.0) bt=0.0;
	else
		bt=exp(gammln(a+b)-gammln(a)-gammln(b)+a*log(x)+b*log(1.0-x));
	if (x < (a+1.0)/(a+b+2.0))
		return bt*betacf(a,b,x)/a;
	else
		return 1.0-bt*betacf(b,a,1.0-x)/b;
}

// From Numerical Recipes in C, p. 227

#define FPMIN 1.0e-30

double
betacf(double a, double b, double x)
{
	int m,m2;
	double aa,c,d,del,h,qab,qam,qap;
	qab=a+b;
	qap=a+1.0;
	qam=a-1.0;
	c=1.0;
	d=1.0-qab*x/qap;
	if (fabs(d) < FPMIN) d=FPMIN;
	d=1.0/d;
	h=d;
	for (m=1;m<=100;m++) {
		m2=2*m;
		aa=m*(b-m)*x/((qam+m2)*(a+m2));
		d=1.0+aa*d;
		if (fabs(d) < FPMIN) d=FPMIN;
		c=1.0+aa/c;
		if (fabs(c) < FPMIN) c=FPMIN;
		d=1.0/d;
		h *= d*c;
		aa = -(a+m)*(qab+m)*x/((a+m2)*(qap+m2));
		d=1.0+aa*d;
		if (fabs(d) < FPMIN) d=FPMIN;
		c=1.0+aa/c;
		if (fabs(c) < FPMIN) c=FPMIN;
		d=1.0/d;
		del=d*c;
		h *= del;
		if (fabs(del-1.0) < 1e-10) break;
	}
	return h;
}
