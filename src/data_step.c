#include "defs.h"

static char lbuf[100]; // level name buffer

int nbytecode;
unsigned char bytecode[10000];

#define TOS 100
double stack[TOS];
int tos;

// dd is the head of the linked list of datsets

static struct dataset *dd;

#define BUFLEN 10001
static char *inb, buf[BUFLEN];
static char filename[1000], delim[100];
static int ctrl;
static int firstobs;
FILE *infile;

enum {
	SCAN_NUM,
	SCAN_STR,
	NEG,
	ADD,
	SUB,
	MUL,
	DIV,
	POW,
	LOG,
	LIT,
	LOAD,
	STORE,
	GETBUF,
	CHKBUF,
};

// The DATALINES statement is the last statement in the DATA step and
// immediately precedes the first data line.

void
data_step(void)
{
	ctrl = 0;
	firstobs = 1;
	nbytecode = 0;
	*filename = 0;

	strcpy(delim, "\t ,");

	parse_data_stmt();

	parse_data_body();
}

// parse statements that follow DATA statement

void
parse_data_body(void)
{
	for (;;) {

		if (token == NAME) {

			// look ahead for NAME = EXPR

			while (*inp == ' ' || *inp == '\t')
				inp++;

			if (*inp == '=') {
				parse_data_expr();
				continue;
			}
		}

		keyword();

		switch (token) {
		case 0:
		case KDATA:
		case KPROC:
		case KRUN:
			if (*filename == 0)
				expected("infile or datalines");
			read_file();
			return;
		case KDATALINES:
			if (*filename)
				stop("Datalines after infile?");
			datalines_stmt();
			return;
		case KINPUT:
			input_stmt();
			break;
		case KINFILE:
			infile_stmt();
			break;
		default:
			parse_default();
			break;
		}
	}
}

void
read_file(void)
{
	infile = fopen(filename, "r");

	if (infile == NULL) {
		sprintf(errbuf, "Cannot open %s", filename);
		stop(errbuf);
	}

	get_data();

	fclose(infile);
	infile = NULL;
}

void
input_stmt(void)
{
	int n;

	scan(); // get next token after INPUT

	// scan data specifiers

	for (;;) {

		if (token != NAME)
			break;

		if (dd->nvar == MAXVAR)
			stop("Too many variables");

		n = dd->nvar++;

		dd->spec[n].name = strdup(strbuf);

		scan();

		if (token == '$') {
			dd->spec[n].max_levels = 10;
			dd->spec[n].num_levels = 0;
			dd->spec[n].ltab = (char **) xmalloc(10 * sizeof (char *));
			emit(SCAN_STR);
			emit(n);
			scan();
		} else {
			dd->spec[n].max_levels = 0;
			dd->spec[n].num_levels = 0;
			dd->spec[n].ltab = NULL;
			emit(SCAN_NUM);
			emit(n);
		}
	}

	if (token == ATAT) {
		emit(CHKBUF);
		scan();
	} else
		emit(GETBUF);

	if (token != ';')
		expected("end of statement");

	scan();
}

void
parse_data_stmt(void)
{
	struct dataset *d;

	d = (struct dataset *) xmalloc(sizeof (struct dataset));

	bzero(d, sizeof (struct dataset));

	d->next = dd;
	dd = d;

	d->max = 1000;

	scan();

	if (token == NAME) {
		d->name = strdup(strbuf); // dataset NAME
		scan();
	}

	if (token != ';')
		expected("end of statment");

	scan();
}

void
infile_stmt(void)
{
	if (filename[0])
		stop("Multiple INFILE statements");

	get_next_token();

	switch (token) {
	case STRING:
		strcpy(filename, strbuf);
		break;
	case KCARDS:
	case KDATALINES:
		break;
	default:
		expected("file name");
	}

	for (;;) {

		scan();
		keyword();

		switch (token) {

		case ';':
			scan();
			return;

		case KDLM:
		case KDELIMITER:
			scan();
			if (token != '=')
				expected("equals sign");
			scan();
			if (token != STRING)
				expected("string");
			if (strlen(strbuf) >= sizeof delim)
				stop("string too long");
			strcpy(delim, strbuf);
			break;

		case KFIRSTOBS:
			scan();
			if (token != '=')
				expected("equals sign");
			scan();
			if (token != NUMBER)
				expected("number");
			if (sscanf(strbuf, "%d", &firstobs) != 1)
				stop("Error in number syntax");
			break;

		default:
			expected("end of statement");
		}
	}
}

void
datalines_stmt(void)
{
	// don't allow garbage after datalines token

	while (isspace(*inp) && *inp != '\n' && *inp != '\r')
		inp++;

	if (*inp == ';') {
		inp++;
		while (isspace(*inp) && *inp != '\n' && *inp != '\r')
			inp++;
	}

	if (*inp != '\n' && *inp != '\r') {
		scan();
		expected("datalines data");
	}

	get_data();

	scan(); // get next token (current token is still DATALINES)
}

void
get_data(void)
{
	alloc_data_vectors();
	getbuf();
	while (inb) {
		check_data_vectors();
		get_data1();
		dd->nobs++;
	}
}

// get one row of observations

void
get_data1(void)
{
	int i, j, n, w, x;

	double t;

	// observation number

	n = dd->nobs;

	// process bytecodes

	tos = 0;

	for (i = 0; i < nbytecode; i++) {
		switch (bytecode[i]) {

		case SCAN_NUM:
			x = bytecode[++i];
			w = get_digits();
			t = get_number();
			if (w > dd->spec[x].w)
				dd->spec[x].w = w;
			dd->spec[x].v[n] = t;
			break;

		case SCAN_STR:
			x = bytecode[++i];
			get_string();
			catvar(x, n);
			break;

		case LOAD:
			if (tos == TOS)
				stop("Stack overflow");
			x = bytecode[++i];
			stack[tos++] = dd->spec[x].v[n];
			break;

		case STORE:
			if (tos == 0)
				stop("Stack underrun");
			x = bytecode[++i];
			t = stack[--tos];
			dd->spec[x].w = 4; // default precision
			dd->spec[x].v[n] = t;
			break;

		case LIT:
			if (tos == TOS)
				stop("Stack overflow");
			for (j = 0; j < sizeof (double); j++)
				((char *) &t)[j] = bytecode[++i];
			stack[tos++] = t;
			break;

		case NEG:
			if (tos == 0)
				stop("Stack underrun");
			stack[tos - 1] = -stack[tos - 1];
			break;

		case ADD:
			if (tos < 2)
				stop("Stack underrun");
			t = stack[--tos];
			stack[tos - 1] += t;
			break;

		case SUB:
			if (tos < 2)
				stop("Stack underrun");
			t = stack[--tos];
			stack[tos - 1] -= t;
			break;

		case MUL:
			if (tos < 2)
				stop("Stack underrun");
			t = stack[--tos];
			stack[tos - 1] *= t;
			break;

		case DIV:
			if (tos < 2)
				stop("Stack underrun");
			t = stack[--tos];
			stack[tos - 1] /= t;
			break;

		case POW:
			if (tos < 2)
				stop("Stack underrun");
			t = stack[--tos];
			stack[tos - 1] = pow(stack[tos - 1], t);
			break;

		case LOG:
			if (tos < 1)
				stop("Stack underrun");
			stack[tos - 1] = log(stack[tos - 1]);
			break;

		case GETBUF:
			getbuf();
			break;

		case CHKBUF:
			chkbuf();
			break;

		default:
			stop("Bytecode error");
		}
	}
}

void
alloc_data_vectors(void)
{
	int i;
	for (i = 0; i < dd->nvar; i++)
		if (dd->spec[i].v == NULL)
			dd->spec[i].v = (double *) xmalloc(dd->max * sizeof (double));
}

// increase length of data vectors if necessary

void
check_data_vectors(void)
{
	int i;
	if (dd->nobs < dd->max)
		return;
	dd->max += 1000;
	for (i = 0; i < dd->nvar; i++)
		dd->spec[i].v = xrealloc(dd->spec[i].v, dd->max * sizeof (double));
}

void
getbuf(void)
{
	do {
		if (infile)
			for (;;) {
				inb = fgets(buf, sizeof buf, infile);
				if (inb == NULL || firstobs < 2)
					break;
				firstobs--;
			}
		else
			inb = get_dataline(buf, sizeof buf);

		if (inb == NULL)
			return;

		// skip spaces

		while (isspace(*inb))
			inb++;

	} while (*inb == 0); // skip blank lines
}

void
chkbuf(void)
{
	// check for end of input

	if (inb == NULL)
		return;

	// skip spaces

	while (isspace(*inb))
		inb++;

	if (*inb == 0)
		getbuf();
}

// get number of decimal digits

int
get_digits(void)
{
	int n = 0;
	char *s = inb;

	// scan to decimal point

	while (strchr(delim, *s) == NULL && *s != '.')
		s++;

	if (*s != '.')
		return 0;

	while (isdigit(*++s) && n < 8)
		n++;

	return n;
}

// get a number from the input buffer

double
get_number(void)
{
	double d = NAN;

	// check end of input

	if (inb == NULL)
		return d;

	// scan leading spaces

	while (isspace(*inb))
		inb++;

	if (*inb == 0)
		return d;

	if (strchr(delim, *inb)) {
		inb++;
		return d;
	}

	// scan number

	sscanf(inb, "%lg", &d);

	// scan delimiter

	while (strchr(delim, *inb) == NULL)
		inb++;

	if (*inb)
		inb++;

	return d;
}

// get a string from the input buffer

void
get_string(void)
{
	int i, n;
	char *s, *t;

	lbuf[0] = 0;

	// check end of input

	if (inb == NULL)
		return;

	// scan leading spaces

	while (isspace(*inb))
		inb++;

	if (*inb == 0)
		return;

	if (strchr(delim, *inb)) {
		inb++;
		return;
	}

	// start of string

	s = inb;

	// find end of string

	while (strchr(delim, *inb) == NULL)
		inb++;

	// skip trailing spaces

	t = inb;
	while (isspace(t[-1]))
		t--;

	n = (int) (t - s);

	if (n + 1 > sizeof lbuf)
		stop("In the observed data, a level name is too long");

	// missing data char?

	if (n == 1 && *s == '.')
		return;

	// copy the string into lbuf

	for (i = 0; i < n; i++)
		lbuf[i] = s[i];

	lbuf[n] = 0;

	// scan delimiter

	if (*inb)
		inb++;
}

void
dump_data(void)
{
	int i, j;
	double d;

	printf("dump_data: %s nvar=%d\n", dd->name, dd->nvar);

	for (i = 0; i < dd->nvar; i++)
		printf(" %s", dd->spec[i].name);

	printf("\n");

	for (i = 0; i < dd->nobs; i++) {
		for (j = 0; j < dd->nvar; j++) {
			d = dd->spec[j].v[i];
			if (dd->spec[j].ltab)
				printf(" %s", dd->spec[j].ltab[(int) d]);
			else
				printf(" %g", d);
		}
		printf("\n");
	}
}

void
free_datasets(void)
{
	int i;
	struct dataset *t;

	while (dd) {
		t = dd;
		dd = dd->next;
		if (t->name)
			free(t->name);
		for (i = 0; i < MAXVAR; i++) {
			if (t->spec[i].name)
				free(t->spec[i].name);
			if (t->spec[i].v)
				free(t->spec[i].v);
			if (t->spec[i].ltab)
				free(t->spec[i].ltab);
		}
		free(t);
	}
}

void
select_dataset(char *s)
{
	dataset = dd;
	if (s == NULL)
		return;
	while (dataset) {
		if (dataset->name && strcmp(dataset->name, s) == 0)
			return;
		dataset = dataset->next;
	}
	sprintf(errbuf, "Dataset %s?", s);
	stop(errbuf);
}

// NAME = EXPR

void
parse_data_expr(void)
{
	if (dd->nvar == MAXVAR)
		stop("Too many variables");
	dd->spec[dd->nvar].name = strdup(strbuf);
	scan(); // skip over name
	scan(); // skip over equals sign
	parse_expr();
	if (token != ';')
		stop("';' expected");
	scan();
	emit(STORE);
	emit(dd->nvar);
	dd->nvar++;
}

void
parse_expr(void)
{
	switch (token) {
	case '+':
		scan();
		parse_term();
		break;
	case '-':
		scan();
		parse_term();
		emit(NEG);
		break;
	default:
		parse_term();
		break;
	}

	for (;;) {
		switch (token) {
		case '+':
			scan();
			parse_term();
			emit(ADD);
			break;
		case '-':
			scan();
			parse_term();
			emit(SUB);
			break;
		default:
			return;
		}
	}
}

void
parse_term(void)
{
	parse_factor();

	for (;;) {
		switch (token) {
		case '*':
			scan();
			parse_factor();
			emit(MUL);
			break;
		case '/':
			scan();
			parse_factor();
			emit(DIV);
			break;
		default:
			return;
		}
	}
}

void
parse_factor(void)
{
	switch (token) {
	case NAME:
		if (strcmp(strbuf, "LOG") == 0)
			parse_log();
		else {
			emit_variable();
			scan();
		}
		break;
	case NUMBER:
		emit_number();
		scan();
		break;
	case '(':
		scan();
		parse_expr();
		if (token != ')')
			stop("Syntax error");
		scan();
		break;
	default:
		stop("Syntax error");
	}

	if (token == STARSTAR) {
		scan();
		switch (token) {
		case '+':
			scan();
			parse_factor();
			break;
		case '-':
			scan();
			parse_factor();
			emit(NEG);
			break;
		default:
			parse_factor();
			break;
		}
		emit(POW);
	}
}

void
parse_log(void)
{
	// look ahead

	while (isspace(*inp) && *inp != '\n' && *inp != '\r')
		inp++;

	if (*inp != '(') {
		emit_variable();
		scan();
		return;
	}

	scan();
	scan();

	parse_expr();

	if (token != ')')
		expected(")");

	scan();

	emit(LOG);
}

void
emit(int c)
{
	if (nbytecode == sizeof bytecode)
		stop("Bytecode buffer overrun");
	bytecode[nbytecode++] = c;
}

// Variable name is in strbuf

void
emit_variable(void)
{
	int i;
	for (i = 0; i < dd->nvar; i++)
		if (strcmp(dd->spec[i].name, strbuf) == 0)
			break;
	if (i == dd->nvar)
		stop("Variable not found");
	if (dd->spec[i].ltab)
		stop("An arithemetic expression cannot use a category variable");
	emit(LOAD);
	emit(i);
}

void
emit_number(void)
{
	int i;
	char *p = (char *) &token_num;
	emit(LIT);
	for (i = 0; i < sizeof (double); i++)
		emit(p[i]);
}

// To here with category level from input data stream
//
//	x	Variable index
//
//	obs	Observation number

void
catvar(int x, int obs)
{
	int i, m, n;

	// missing data?

	if (lbuf[0] == 0) {
		dd->spec[x].v[obs] = NAN;
		return;
	}

	m = dd->spec[x].max_levels;
	n = dd->spec[x].num_levels;

	// search for the level name

	for (i = 0; i < n; i++)
		if (strcmp(dd->spec[x].ltab[i], lbuf) == 0)
			break;

	// check for new level name

	if (i == n) {

		// get space for more levels if necessary

		if (m == n) {
			m += 100;
			dd->spec[x].max_levels = m;
			dd->spec[x].ltab = xrealloc(dd->spec[x].ltab, m * sizeof (char *));
		}

		dd->spec[x].ltab[i] = strdup(lbuf);
		dd->spec[x].num_levels++;
	}

	// save the level number

	dd->spec[x].v[obs] = i;
}
