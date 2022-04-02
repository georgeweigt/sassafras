#include <stdio.h>
#include "defs.h"

void
comment_stmt(void)
{
	while (*inp) {

		// semicolon or end of line terminates comment statement

		if (*inp == ';' || *inp == '\n' || *inp == '\r')
			break;

		inp++;
	}

	scan();
}
