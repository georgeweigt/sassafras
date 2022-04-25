#include <stdio.h>
#include <stdlib.h>

char s[100];

int f(int);

int
main()
{
	int i = 0;
	while (f(++i))
		;
//	printf("%d test files checked and passed\n", i - 1);
}

int
f(int i)
{
	int n;
	FILE *f;

	sprintf(s, "test%d.in", i);
	f = fopen(s, "r");
	fclose(f);

	if (f == NULL)
		return 0;

	sprintf(s, "../src/sassafras test%d.in | diff - test%d.out", i, i);
	printf("%s\n", s);

	n = system(s);

//	if (n) {
//		printf("diff returned %d\n", n);
//		exit(1);
//	}

	return 1;
}
