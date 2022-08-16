#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

char *
read_text_file(char *filename)
{
	int fd, n;
	char *buf;

	fd = open(filename, O_RDONLY);

	if (fd == -1) {
		fprintf(stderr, "cannot open %s\n", filename);
		exit(1);
	}

	n = lseek(fd, 0, SEEK_END);

	if (n < 0)
		exit(1);

	if (lseek(fd, 0, SEEK_SET))
		exit(1);

	buf = malloc(n + 1);

	if (buf == NULL)
		exit(1);

	if (read(fd, buf, n) != n)
		exit(1);

	close(fd);

	buf[n] = '\0';

	return buf;
}
