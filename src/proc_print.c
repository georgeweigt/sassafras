void
proc_print(void)
{
	parse_proc_print_stmt();

	if (dataset == NULL)
		stop("No data set");

	parse_proc_print_body();

	run_proc_print();
}

void
parse_proc_print_stmt(void)
{
	for (;;) {
		get_next_token();
		switch (token) {
		case ';':
			scan();
			return;
		case KDATA:
			parse_data_option();
			break;
		default:
			stop("';' expected");
		}
	}
}

// parse statements that follow PROC PRINT

void
parse_proc_print_body(void)
{
	for (;;) {
		keyword();
		switch (token) {
		case 0:
		case KDATA:
		case KPROC:
		case KRUN:
			return;
		case KVAR:
			parse_var_stmt();
			break;
		default:
			parse_default();
			break;
		}
	}
}

#define A(i, j) (a + (i) * (nvar + 1))[j]

void
run_proc_print(void)
{
	int i, j, nobs, w, x;
	double d;
	char **a, *s;

	print_title();

	nobs = dataset->nobs;

	// print all variables if not specified with VAR

	if (nvar == 0) {
		nvar = dataset->nvar;
		for (i = 0; i < nvar; i++)
			var[i] = i;
	}

	// create array

	a = (char **) xmalloc((nobs + 1) * (nvar + 1) * sizeof (char *));

	// column names

	A(0, 0) = strdup("Obs");

	for (j = 0; j < nvar; j++) {
		x = var[j];
		A(0, j + 1) = strdup(dataset->spec[x].name);
	}

	// for each row

	for (i = 0; i < nobs; i++) {

		// observation number

		snprintf(bigbuf, BIGBUFLEN, "%d", i + 1);

		A(i + 1, 0) = strdup(bigbuf);

		// for each variable

		for (j = 0; j < nvar; j++) {

			x = var[j];

			d = dataset->spec[x].v[i];

			if (isnan(d))
				s = ".";
			else {
				if (dataset->spec[x].ltab == NULL) {
					w = dataset->spec[x].w;
					snprintf(bigbuf, BIGBUFLEN, fmt[w], d);
					s = bigbuf;
				} else
					s = dataset->spec[x].ltab[(int) d];
			}

			A(i + 1, j + 1) = strdup(s);
		}
	}

	s = (char *) xmalloc(nvar + 1);

	s[0] = 0; // observation numbers are right justified

	for (j = 0; j < nvar; j++) {
		x = var[j];
		if (dataset->spec[x].ltab == NULL)
			s[j + 1] = 0;
		else
			s[j + 1] = 1;
	}

	print_table(a, nobs + 1, nvar + 1, s);

	free(s);

	for (i = 0; i < nobs + 1; i++)
		for (j = 0; j < nvar + 1; j++)
			free(A(i, j));

	free(a);
}

#undef A
