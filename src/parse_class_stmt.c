#include "defs.h"

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
