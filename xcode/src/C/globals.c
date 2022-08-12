#include "defs.h"

int total_h;
struct display *display_list;

char *inp;
char *token_str;
int token;
double token_num;

char errbuf[1000];

// options

struct dataset *dataset;
double alpha;
int maxdec;
char *title;
char *title1;
char *title2;
char *title3;
int nby;
int by[MAXVAR];
int nclass;
int class[MAXVAR];
int nstat;
int stat[MAXSTAT];
int nvar;
int var[MAXVAR];
