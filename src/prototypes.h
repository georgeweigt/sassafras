
// data_step.c
void data_step(void);
void parse_data_body();
void read_file(void);
void input_stmt(void);
void parse_data_stmt();
void infile_stmt(void);
void datalines_stmt(void);
void get_data(void);
void get_data1(void);
void alloc_data_vectors(void);
void check_data_vectors(void);
void getbuf();
void chkbuf();
int get_digits(void);
double get_number(void);
void get_string();
void dump_data(void);
void free_datasets();
void select_dataset(char *s);
void parse_data_expr();
void parse_expr();
void parse_term();
void parse_factor();
void parse_log();
void emit(int c);
void emit_variable();
void emit_number();
void catvar(int x, int obs);

// main.c
int main(int argc, char **argv);
void run();
void run1();
void procedure_step(void);
void parse_default();
void parse_comment_stmt(void);
void parse_title_stmt();
void parse_title1_stmt();
void parse_title2_stmt();
void parse_title3_stmt();
void parse_alpha_option();
void parse_data_option();
void parse_maxdec_option();
void parse_by_stmt(void);
void parse_class_stmt(void);
void parse_var_stmt(void);
void print_table_and_free(char **a, int nrow, int ncol, char *fmt);
void print_table(char **a, int nrow, int ncol, char *fmt);
void print_title();
void emit_line(char *s);
void emit_line_center(char *s);
void * xmalloc(int size);
void * xcalloc(int size);
void * xrealloc(void *p, int size);
void print_pgm();
void stop(char *s);
void expected(char *s);

// proc_anova.c
void proc_anova();
void print_anova_table_part1();
void print_anova_table_part2();
void print_anova_table_part3();

// proc_means.c
void proc_means();
void parse_proc_means_stmt();
void parse_proc_means_body();
void run_proc_means();
void f(int k);
void g();
void h(int varnum);

// proc_print.c
void proc_print();
void parse_proc_print_stmt(void);
void parse_proc_print_body(void);
void run_proc_print(void);

// proc_reg.c
void proc_reg();
void compute_X();
void compute_T();
int compute_G();
void compute_mse();
void compute_C();
void compute_SE();
void compute_TVAL();
void compute_PVAL();
void print_B();
void print_X();
void print_T();
void print_G();
void print_Z();
void regress();
void print_parameter_estimates();
void print_anova_table();
void print_diag_table();
void print_diag_table();

// scan.c
void scan(void);
int scan1(void);
void keyword(void);
char * get_dataline(char *buf, int len);
void get_next_token(void);

// tdist.c
double qt(double p, double df);
double tdist(double t, double df);
double qf(double p, double df1, double df2);
double fdist(double t, double df1, double df2);
double gammln(double xx);
double betai(double a, double b, double x);
double betacf(double a, double b, double x);
