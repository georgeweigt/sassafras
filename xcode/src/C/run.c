#include "defs.h"

char *pgm;
static jmp_buf jmpbuf;

void
run(char *s)
{
	if (s == NULL)
		return;

	emit_line_init();

	run_nib(s);

	// clean up (always gets here, even on stop)

	if (infile) {
		fclose(infile);
		infile = NULL;
	}

	free_datasets();

	FREE(title)
	FREE(title1)
	FREE(title2)
	FREE(title3)
}

void
run_nib(char *s)
{
	if (setjmp(jmpbuf))
		return;

	pgm = s;

	inp = s;

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
			proc_step();
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
parse_default(void)
{
	switch (token) {
	case ';':
		scan();
		break;
	case '*':
		parse_comment_stmt();
		break;
	case KTITLE:
		title_stmt();
		break;
	case KTITLE1:
		title1_stmt();
		break;
	case KTITLE2:
		title2_stmt();
		break;
	case KTITLE3:
		title3_stmt();
		break;
	default:
		stop("Unexpected token");
	}
}

void
parse_alpha_option(void)
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
parse_data_option(void)
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
parse_maxdec_option(void)
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

void *
xmalloc(int size)
{
	void *p = malloc(size);
	if (p == NULL)
		exit(1);
	return p;
}

void *
xrealloc(void *p, int size)
{
	p = realloc(p, size);
	if (p == NULL)
		exit(1);
	return p;
}

void
print_pgm(void)
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
