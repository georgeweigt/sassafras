#include <stdio.h>
#include <stdlib.h> // for system()
#include <string.h>
#include <sys/types.h>
#include <dirent.h>

int filter(const struct dirent *p);
int check(char *);

int
main()
{
	int count = 0, i, n;
	struct dirent **p;

	n = scandir(".", &p, filter, alphasort);

	for (i = 0; i < n; i++)
		count += check(p[i]->d_name);

	printf("%d diffs failed\n", count);
}

int
filter(const struct dirent *p)
{
	int len = strlen(p->d_name);
	return len > 3 && strcmp(p->d_name + len - 3, ".in") == 0;
}

int
check(char *filename)
{
	char *s;
	static char buf[1000];
	s = strchr(filename, '.');
	*s = '\0';
	sprintf(buf, "../src/sassafras %s.in | diff - %s.out", filename, filename);
	printf("%s\n", buf);
	return system(buf) != 0;
}
