#include "defs.h"

void
var_stmt(void)
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
