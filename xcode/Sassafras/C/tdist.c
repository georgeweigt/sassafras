#include "defs.h"

double
LogGamma(double x)
{
	double s;
	s = 1+76.18009173/x-86.50532033/(x+1)+24.01409822/(x+2)-1.231739516/(x+3)+.00120858003/(x+4)-.00000536382/(x+5);
	return (x-.5)*log(x+4.5)-(x+4.5)+log(s*2.50662827465);
}

double
Betinc(double X, double A, double B)
{
	double A0, B0, A1, B1, M9, A2, C9;
	A0=0;
	B0=1;
	A1=1;
	B1=1;
	M9=0;
	A2=0;
	while (fabs((A1-A2)/A1)>.00001) {
		A2=A1;
		C9=-(A+M9)*(A+B+M9)*X/(A+2*M9)/(A+2*M9+1);
		A0=A1+C9*A0;
		B0=B1+C9*B0;
		M9=M9+1;
		C9=M9*(B-M9)*X/(A+2*M9-1)/(A+2*M9);
		A1=A0+C9*A1;
		B1=B0+C9*B1;
		A0=A0/B1;
		B0=B0/B1;
		A1=A1/B1;
		B1=1;
	}
	return A1/A;
}

// CDF, like pt() in R

double
tdist(double X, double df)
{
	double A,S,Z,BT,betacdf,tcdf;
	if (df<=0) {
		return -1.0;
	} else {
		A=df/2;
		S=A+.5;
		Z=df/(df+X*X);
		BT=exp(LogGamma(S)-LogGamma(.5)-LogGamma(A)+A*log(Z)+.5*log(1-Z));
		if (Z<(A+1)/(S+2)) {
			betacdf=BT*Betinc(Z,A,.5);
		} else {
			betacdf=1-BT*Betinc(1-Z,.5,A);
		}
		if (X<0) {
			tcdf=betacdf/2;
		} else {
			tcdf=1-betacdf/2;
		}
	}
	return tcdf;
}

// Inverse CDF, like qt() in R

double
qt(double p, double df)
{
	int i;
	double a, t, t1, t2;
	if (isnan(p) || isnan(df) || df < 1)
		return NAN;
	t1 = -1000;
	t2 = 1000;
	for (i = 0; i < 100; i++) {
		t = (t1 + t2) / 2;
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

double
Betacdf(double Z, double A, double B)
{
	double S;
	double BT;
	double Bcdf;
	S=A+B;
	BT=exp(LogGamma(S)-LogGamma(B)-LogGamma(A)+A*log(Z)+B*log(1-Z));
	if (Z<(A+1)/(S+2)) {
		Bcdf=BT*Betinc(Z,A,B);
	} else {
		Bcdf=1-BT*Betinc(1-Z,B,A);
	}
	return Bcdf;
}

double
fdist(double x, double df1, double df2)
{
	double z = 0;

	if (x > 0) {
		z = x / (x + df2 / df1);
		z = Betacdf(z, df1 / 2, df2 / 2);
	}

	return z;
}

double
qf(double p, double df1, double df2)
{
	int i;
	double a, t, t1, t2;
	if (isnan(p) || isnan(df1) || isnan(df2) || df1 < 1 || df2 < 1)
		return NAN;
	t1 = 0;
	t2 = 1000;
	for (i = 0; i < 100; i++) {
		t = (t1 + t2) / 2;
		a = fdist(t, df1, df2);
		if (fabs(a - p) < 1e-10)
			break;
		if (a < p)
			t1 = t;
		else
			t2 = t;
	}
	return t;
}
