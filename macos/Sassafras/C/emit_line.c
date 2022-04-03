#include "defs.h"

char *output_buffer;
int output_buffer_index;
int output_buffer_length;

void
emit_line(char *s)
{
	int len = (int) strlen(s);

	if (output_buffer_index + len + 2 > output_buffer_length) {
		output_buffer_length += len + 10000;
		output_buffer = realloc(output_buffer, output_buffer_length);
		if (output_buffer == NULL) {
			fprintf(stderr, "malloc kaput\n");
			exit(1);
		}
	}

	strcpy(output_buffer + output_buffer_index, s);
	output_buffer_index += len;

	strcpy(output_buffer + output_buffer_index, "\n");
	output_buffer_index += 1;
}

void
emit_line_center(char *s)
{
	int i, len, m;

	len = (int) strlen(s);

	m = (80 - len) / 2;

	if (m < 0)
		m = 0;

	if (output_buffer_index + m + len + 2 > output_buffer_length) {
		output_buffer_length += len + 10000;
		output_buffer = realloc(output_buffer, output_buffer_length);
		if (output_buffer == NULL) {
			fprintf(stderr, "malloc kaput\n");
			exit(1);
		}
	}

	for (i = 0; i < m; i++)
		output_buffer[output_buffer_index + i] = ' ';

	output_buffer_index += m;

	strcpy(output_buffer + output_buffer_index, s);
	output_buffer_index += len;

	strcpy(output_buffer + output_buffer_index, "\n");
	output_buffer_index += 1;
}
