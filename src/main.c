#include "defs.h"

int
main(int argc, char **argv)
{
	char *s;

	if (argc < 2) {
		fprintf(stderr, "usage: sassafras filename\n");
		return 0;
	}

	s = read_text_file(argv[1]);

	run(s);

	free(s);
}

void
emit_line_init(void)
{
}

void
emit_line(char *s)
{
	printf("%s\n", s);
}

void
emit_line_center(char *s)
{
	int i, n;

	n = strlen(s);

	n = (80 - n) / 2;

	for (i = 0; i < n; i++)
		printf(" ");

	printf("%s\n", s);
}
