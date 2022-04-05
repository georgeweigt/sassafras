#include "defs.h"

char *pgm;
char errbuf[1000];
char *inp;
char *token_str;
int token;
double token_num;
char *prefix = ".";
jmp_buf jmpbuf;

struct dataset *dataset;
double alpha;
int maxdec;
char *title;
char *title1;
char *title2;
char *title3;
int nby;
int by[MAXVAR];
int nclass;
int class[MAXVAR];
int nstat;
int stat[MAXSTAT];
int nvar;
int var[MAXVAR];

int
main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "usage: sassafras filename\n");
		return 0;
	}
	pgm = read_text_file(argv[1]);
	if (pgm == NULL)
		fprintf(stderr, "error reading %s\n", argv[1]);
	else
		run();
	return 0;
}

void
run()
{
	run1();
	if (infile) {
		fclose(infile);
		infile = NULL;
	}
	free_datasets();
}

void
run1()
{
	if (setjmp(jmpbuf))
		return;
	inp = pgm;
	scan();
	for (;;) {
		keyword();
		switch (token) {
		case 0:
			return;
		case KDATA:
			data_step();
			break;
		case KPROC:
			procedure_step();
			break;
		case KRUN:
			scan();
			if (token != ';')
				stop("';' expected");
			scan();
			break;
		default:
			parse_default();
			break;
		}
	}
}

void
procedure_step(void)
{
	nby = 0;
	nvar = 0;
	nstat = 0;
	nclass = 0;

	maxdec = 3;
	alpha = 0.05;

	select_dataset(NULL);

	scan(); // get token after PROC

	keyword();

	switch (token) {
	case KANOVA:
		proc_anova();
		break;
	case KMEANS:
		proc_means();
		break;
	case KPRINT:
		proc_print();
		break;
	case KREG:
		proc_reg();
		break;
	default:
		expected("procedure name");
	}
}

void
parse_default()
{
	switch (token) {
	case ';':
		scan();
		break;
	case '*':
		parse_comment_stmt();
		break;
	case KTITLE:
		parse_title_stmt();
		break;
	case KTITLE1:
		parse_title1_stmt();
		break;
	case KTITLE2:
		parse_title2_stmt();
		break;
	case KTITLE3:
		parse_title3_stmt();
		break;
	default:
		stop("Unexpected token");
	}
}

void
parse_comment_stmt(void)
{
	while (*inp) {

		// semicolon or end of line terminates comment statement

		if (*inp == ';' || *inp == '\n' || *inp == '\r')
			break;

		inp++;
	}

	scan();
}

void
parse_title_stmt()
{
	scan();
	if (token != STRING)
		expected("string");
	if (title)
		free(title);
	if (strbuf[0] == 0)
		title = NULL;
	else
		title = strdup(strbuf);
	scan();
	if (token != ';')
		stop("';' expected");
	scan();
}

void
parse_title1_stmt()
{
	scan();
	if (token != STRING)
		expected("string");
	if (title1)
		free(title1);
	if (strbuf[0] == 0)
		title1 = NULL;
	else
		title1 = strdup(strbuf);
	scan();
	if (token != ';')
		stop("';' expected");
	scan();
}

void
parse_title2_stmt()
{
	scan();
	if (token != STRING)
		expected("string");
	if (title2)
		free(title2);
	if (strbuf[0] == 0)
		title2 = NULL;
	else
		title2 = strdup(strbuf);
	scan();
	if (token != ';')
		stop("';' expected");
	scan();
}

void
parse_title3_stmt()
{
	scan();
	if (token != STRING)
		expected("string");
	if (title3)
		free(title3);
	if (strbuf[0] == 0)
		title3 = NULL;
	else
		title3 = strdup(strbuf);
	scan();
	if (token != ';')
		stop("';' expected");
	scan();
}

void
parse_alpha_option()
{
	scan(); // skip alpha token

	if (token != '=')
		expected("equals sign");

	scan(); // skip equals sign

	if (token != NUMBER)
		expected("number");

	alpha = atof(strbuf);

	if (alpha < 0 || alpha > 1)
		expected("number between 0 and 1");
}

void
parse_data_option()
{
	scan();
	if (token != '=')
		stop("'=' expected");
	scan();
	if (token != NAME)
		stop("DATA=NAME expected");
	select_dataset(strbuf);
}

void
parse_maxdec_option()
{
	scan();
	if (token != '=')
		stop("'=' expected");

	scan();
	if (token != NUMBER)
		stop("Number expected");

	maxdec = (int) token_num;

	if (maxdec < 0)
		maxdec = 0;

	if (maxdec > 8)
		maxdec = 8;
}

void
parse_by_stmt(void)
{
	int i;

	for (;;) {

		scan();

		if (token == ';')
			break;

		if (token != NAME)
			stop("Variable name expected");

		// look for match in dataset

		for (i = 0; i < dataset->nvar; i++)
			if (strcmp(dataset->spec[i].name, strbuf) == 0)
				break;

		if (i == dataset->nvar) {
			sprintf(errbuf, "The variable %s not in the dataset", strbuf);
			stop(errbuf);
		}

		if (dataset->spec[i].ltab == NULL) {
			sprintf(errbuf, "The variable %s is not a categorical variable", strbuf);
			stop(errbuf);
		}

		by[nby++] = i;
	}

	scan(); // eat the semicolon
}

void
parse_class_stmt(void)
{
	int i;

	for (;;) {

		scan();

		if (token == ';')
			break;

		if (token != NAME)
			stop("Variable name expected");

		// look for match in dataset

		for (i = 0; i < dataset->nvar; i++)
			if (strcmp(dataset->spec[i].name, strbuf) == 0)
				break;

		if (i == dataset->nvar) {
			sprintf(errbuf, "The variable %s not in the dataset", strbuf);
			stop(errbuf);
		}

		if (dataset->spec[i].ltab == NULL) {
			sprintf(errbuf, "The variable %s is not a categorical variable", strbuf);
			stop(errbuf);
		}

		class[nclass++] = i;
	}

	scan(); // eat the semicolon
}

void
parse_var_stmt(void)
{
	int i;

	for (;;) {

		scan();

		if (token == ';')
			break;

		if (token != NAME)
			stop("Variable name expected");

		// look for match in dataset

		for (i = 0; i < dataset->nvar; i++)
			if (strcmp(dataset->spec[i].name, strbuf) == 0)
				break;

		if (i == dataset->nvar) {
			sprintf(errbuf, "Variable %s?", strbuf);
			stop(errbuf);
		}

		var[nvar++] = i;
	}

	scan(); // eat the semicolon
}

#define A(i, j) (a + (i) * ncol)[j]

void
print_table_and_free(char **a, int nrow, int ncol, char *fmt)
{
	int i, j;
	print_table(a, nrow, ncol, fmt);
	for (i = 0; i < nrow; i++)
		for (j = 0; j < ncol; j++)
			free(A(i, j));
	free(a);
}

void
print_table(char **a, int nrow, int ncol, char *fmt)
{
	int c, i, j, k, n, nsp = 0, t, *w;
	char *b, *buf, *s;

	w = (int *) xmalloc(ncol * sizeof (int));

	// measure column widths

	t = 0;

	for (j = 0; j < ncol; j++) {
		w[j] = 0;
		for (i = 0; i < nrow; i++) {
			s = A(i, j);
			if (s == NULL)
				continue;
			n = (int) strlen(s);
			if (n > w[j])
				w[j] = n;
		}
		t += w[j];
	}

	// spaces between columns

	if (ncol > 1) {
		nsp = (80 - t) / (ncol - 1);
		if (nsp < 2)
			nsp = 2;
		if (nsp > 5)
			nsp = 5;
		t += (ncol - 1) * nsp;
	}

	// number of spaces to center the line

	if (t < 80)
		c = (80 - t) / 2;
	else
		c = 0;

	// alloc line buffer

	buf = (char *) xmalloc(c + t + 1);

	// leading spaces

	memset(buf, ' ', c);

	// for each row

	for (i = 0; i < nrow; i++) {

		b = buf + c;

		// for each column

		for (j = 0; j < ncol; j++) {

			// space between columns

			if (j > 0) {
				memset(b, ' ', nsp);
				b += nsp;
			}

			s = A(i, j);

			if (s == NULL) {
				memset(b, ' ', w[j]);
				b += w[j];
			} else {

				n = (int) strlen(s);

				k = w[j] - n;

				switch (fmt[j]) {

				// right aligned

				case 0:
					memset(b, ' ', k);
					b += k;
					strcpy(b, s);
					b += n;
					break;

				// left aligned

				case 1:
					strcpy(b, s);
					b += n;
					memset(b, ' ', k);
					b += k;
					break;
				}
			}
		}

		*b = 0;

		emit_line(buf);
	}

	free(buf);
	free(w);

	emit_line("");
}

void
print_title()
{
	if (title)
		emit_line_center(title);

	if (title1)
		emit_line_center(title1);

	if (title2)
		emit_line_center(title2);

	if (title3)
		emit_line_center(title3);

	if (title || title1 || title2 || title3)
		emit_line("");
}

void
emit_line(char *s)
{
	printf("%s\n", s);
}

void
emit_line_center(char *s)
{
	int i, n;

	n = strlen(s);

	n = (80 - n) / 2;

	for (i = 0; i < n; i++)
		printf(" ");

	printf("%s\n", s);
}

void *
xmalloc(int size)
{
	void *p = malloc(size);
	if (p == NULL)
		stop("Out of memory");
	return p;
}

void *
xcalloc(int size)
{
	void *p = calloc(1, size);
	if (p == NULL)
		stop("Out of memory");
	return p;
}

void *
xrealloc(void *p, int size)
{
	p = realloc(p, size);
	if (p == NULL)
		stop("Out of memory");
	return p;
}

void
print_pgm()
{
	char c, *s, *t;

	s = pgm;

	for (;;) {

		while (*s && s != inp && (*s == '\n' || *s == '\r'))
			s++;

		if (*s == 0 || s == inp)
			break;

		t = s;

		while (*s && s != inp && *s != '\n' && *s != '\r')
			s++;

		c = *s;
		*s = 0;
		emit_line(t);
		*s = c;
	}
}

void
stop(char *s)
{
	print_pgm();
	emit_line(s);
	longjmp(jmpbuf, 1);
}

void
expected(char *s)
{
	if (token == 0)
		sprintf(errbuf, "Expected %s before end of program", s);
	else if (token == ';')
		sprintf(errbuf, "Expected %s before end of statement", s);
	else
		sprintf(errbuf, "Expected %s instead of \"%s\"", s, strbuf);
	stop(errbuf);
}
