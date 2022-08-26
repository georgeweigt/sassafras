#include "defs.h"

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
