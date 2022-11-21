#include "defs.h"

char *output_buffer;
int output_buffer_index;
int output_buffer_length;

void
emit_line_init(void)
{
	output_buffer_index = 0;
	emit_line(""); // make realloc happen if first time
	output_buffer_index = 0;
}

void
emit_line(char *s)
{
	int len, m;

	len = (int) strlen(s);

	// Let output_buffer_index + len + 1 == 1000 (added 1 for newline)

	// Then m == 2000 hence there is always room for the terminator '\0'

	m = 1000 * ((output_buffer_index + len + 1) / 1000 + 1); // m is a multiple of 1000

	if (m > output_buffer_length) {
		output_buffer = realloc(output_buffer, m);
		if (output_buffer == NULL)
			exit(1);
		output_buffer_length = m;
	}

	strcpy(output_buffer + output_buffer_index, s);
	output_buffer_index += len;

	strcpy(output_buffer + output_buffer_index, "\n");
	output_buffer_index += 1;
}

void
emit_line_center(char *s)
{
	int i, len, m, n = 0;

	len = (int) strlen(s);

	if (len < 80)
		n = (80 - len) / 2;

	// Let output_buffer_index + n + len + 1 == 1000 (added 1 for newline)

	// Then m == 2000 hence there is always room for the terminator '\0'

	m = 1000 * ((output_buffer_index + n + len + 1) / 1000 + 1); // m is a multiple of 1000

	if (m > output_buffer_length) {
		output_buffer = realloc(output_buffer, m);
		if (output_buffer == NULL)
			exit(1);
		output_buffer_length = m;
	}

	for (i = 0; i < n; i++)
		output_buffer[output_buffer_index++] = ' ';

	strcpy(output_buffer + output_buffer_index, s);
	output_buffer_index += len;

	strcpy(output_buffer + output_buffer_index, "\n");
	output_buffer_index += 1;
}
