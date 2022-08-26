#include "defs.h"

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
