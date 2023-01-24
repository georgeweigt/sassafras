jmp_buf jmpbuf;

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

	xfree(title);
	xfree(title1);
	xfree(title2);
	xfree(title3);
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
xfree(void *p)
{
	if (p)
		free(p);
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
		snprintf(errbuf, ERRBUFLEN, "Expected %s before end of program", s);
	else if (token == ';')
		snprintf(errbuf, ERRBUFLEN, "Expected %s before end of statement", s);
	else
		snprintf(errbuf, ERRBUFLEN, "Expected %s instead of \"%s\"", s, strbuf);
	stop(errbuf);
}
