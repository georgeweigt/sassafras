#include "defs.h"

int noint;

int num_y;
int ytab[MAXVAR];

int num_x;
int xtab[MAXVAR];

static void parse_model_options(void);

void
model_stmt(void)
{
	int i;

	noint = 0;

	num_x = 0;
	num_y = 0;

	scan();

	if (token != NAME)
		expected("variable name");

	// look for match in dataset

	for (i = 0; i < dataset->nvar; i++)
		if (strcmp(dataset->spec[i].name, strbuf) == 0)
			break;

	if (i == dataset->nvar) {
		sprintf(errbuf, "Variable %s not in dataset", strbuf);
		stop(errbuf);
	}

	ytab[num_y++] = i;

	scan();

	if (token != '=')
		expected("equals sign");

	for (;;) {

		scan();

		if ((token == ';' || token == '/') && num_x)
			break;

		if (token != NAME)
			expected("variable name");

		// look for match in dataset

		for (i = 0; i < dataset->nvar; i++)
			if (strcmp(dataset->spec[i].name, strbuf) == 0)
				break;

		if (i == dataset->nvar) {
			sprintf(errbuf, "Variable %s not in dataset", strbuf);
			stop(errbuf);
		}

		xtab[num_x++] = i;
	}

	if (token == '/')
		parse_model_options();

	scan(); // eat the semicolon
}

static void
parse_model_options()
{
	for (;;) {

		scan();

		keyword();

		switch (token) {

		case ';':
			return;

		case KNOINT:
			noint = 1;
			break;

		default:
			stop("';' or MODEL option expected");
		}
	}
}
