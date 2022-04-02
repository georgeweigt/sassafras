#include "defs.h"

void print_title(void);
void emit_line(char *);
void emit_line_center(char *);

#define A(i, j) (a + (i) * ncol)[j]

void
print_table_and_free(char **a, int nrow, int ncol, char *fmt)
{
	int i, j;
	print_table(a, nrow, ncol, fmt);
	for (i = 0; i < nrow; i++)
		for (j = 0; j < ncol; j++)
			free(A(i, j));
	free(a);
}

void
print_table(char **a, int nrow, int ncol, char *fmt)
{
	int c, i, j, k, n, nsp = 0, t, *w;
	char *b, *buf, *s;

	w = (int *) xmalloc(ncol * sizeof (int));

	// measure column widths

	t = 0;

	for (j = 0; j < ncol; j++) {
		w[j] = 0;
		for (i = 0; i < nrow; i++) {
			s = A(i, j);
			if (s == NULL)
				continue;
			n = (int) strlen(s);
			if (n > w[j])
				w[j] = n;
		}
		t += w[j];
	}

	// spaces between columns

	if (ncol > 1) {
		nsp = (80 - t) / (ncol - 1);
		if (nsp < 2)
			nsp = 2;
		if (nsp > 5)
			nsp = 5;
		t += (ncol - 1) * nsp;
	}

	// number of spaces to center the line

	if (t < 80)
		c = (80 - t) / 2;
	else
		c = 0;

	// alloc line buffer

	buf = (char *) xmalloc(c + t + 1);

	// leading spaces

	memset(buf, ' ', c);

	// for each row

	for (i = 0; i < nrow; i++) {

		b = buf + c;

		// for each column

		for (j = 0; j < ncol; j++) {

			// space between columns

			if (j > 0) {
				memset(b, ' ', nsp);
				b += nsp;
			}

			s = A(i, j);

			if (s == NULL) {
				memset(b, ' ', w[j]);
				b += w[j];
			} else {

				n = (int) strlen(s);

				k = w[j] - n;

				switch (fmt[j]) {

				// right aligned

				case 0:
					memset(b, ' ', k);
					b += k;
					strcpy(b, s);
					b += n;
					break;

				// left aligned

				case 1:
					strcpy(b, s);
					b += n;
					memset(b, ' ', k);
					b += k;
					break;
				}
			}
		}

		*b = 0;

		emit_line(buf);
	}

	free(buf);
	free(w);

	emit_line("");
}

void
print_title()
{
	if (title)
		emit_line_center(title);

	if (title1)
		emit_line_center(title1);

	if (title2)
		emit_line_center(title2);

	if (title3)
		emit_line_center(title3);

	if (title || title1 || title2 || title3)
		emit_line("");
}
