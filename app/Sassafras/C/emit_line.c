#include "defs.h"

static int len;
char output_c_string[100000];

void
emit_line(char *s)
{
	int n = (int) strlen(s);
	if (len + n + 1 > sizeof output_c_string)
		return;
	strcpy(output_c_string + len, s);
	len += n;
	strcpy(output_c_string + len, "\n");
	len++;
}

void
emit_line_center(char *s)
{
	int i, m, n;

	n = (int) strlen(s);

	m = (80 - n) / 2;

	if (m < 0)
		m = 0;

	if (len + m + n + 1 > sizeof output_c_string)
		return;

	for (i = 0; i < m; i++)
		output_c_string[len + i] = ' ';

	len += m;

	strcpy(output_c_string + len, s);
	len += n;
	strcpy(output_c_string + len, "\n");
	len++;
}

void
clear_display()
{
	len = 0;
	output_c_string[0] = '\0';
}
