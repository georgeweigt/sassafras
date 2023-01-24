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
		stop("Unexpected token, did you mean proc?");
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
			snprintf(errbuf, ERRBUFLEN, "The variable %s not in the dataset", strbuf);
			stop(errbuf);
		}

		if (dataset->spec[i].ltab == NULL) {
			snprintf(errbuf, ERRBUFLEN, "The variable %s is not a categorical variable", strbuf);
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
			snprintf(errbuf, ERRBUFLEN, "The variable %s not in the dataset", strbuf);
			stop(errbuf);
		}

		if (dataset->spec[i].ltab == NULL) {
			snprintf(errbuf, ERRBUFLEN, "The variable %s is not a categorical variable", strbuf);
			stop(errbuf);
		}

		class[nclass++] = i;
	}

	scan(); // eat the semicolon
}

void
parse_comment_stmt(void)
{
	while (*inp && *inp != ';')
		inp++;
	scan();
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
			snprintf(errbuf, ERRBUFLEN, "Variable %s?", strbuf);
			stop(errbuf);
		}

		var[nvar++] = i;
	}

	scan(); // eat the semicolon
}
