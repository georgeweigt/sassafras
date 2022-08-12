void data_step(void);
void parse_data_body(void);
void read_file(void);
void input_stmt(void);
void parse_data_stmt(void);
void infile_stmt(void);
void datalines_stmt(void);
void get_data(void);
void get_data1(void);
void alloc_data_vectors(void);
void check_data_vectors(void);
void getbuf(void);
void chkbuf(void);
int get_digits(void);
double get_number(void);
void get_string(void);
void dump_data(void);
void free_datasets(void);
void select_dataset(char *s);
void parse_data_expr(void);
void parse_expr(void);
void parse_term(void);
void parse_factor(void);
void parse_log(void);
void emit(int c);
void emit_variable(void);
void emit_number(void);
void catvar(int x, int obs);
void emit_line(char *s);
void emit_line_center(char *s);
void model_stmt(void);
void parse_by_stmt(void);
void parse_class_stmt(void);
void parse_comment_stmt(void);
void parse_var_stmt(void);
void print_table_and_free(char **a, int nrow, int ncol, char *fmt);
void print_table(char **a, int nrow, int ncol, char *fmt);
void print_title(void);
void proc_anova(void);
void print_anova_table_part1(void);
void print_anova_table_part2(void);
void print_anova_table_part3(void);
void proc_means(void);
void parse_proc_means_stmt(void);
void parse_proc_means_body(void);
void run_proc_means(void);
void f(int k);
void g(void);
void h(int varnum);
void proc_print(void);
void parse_proc_print_stmt(void);
void parse_proc_print_body(void);
void run_proc_print(void);
void proc_reg(void);
void compute_X(void);
void compute_T(void);
int compute_G(void);
void compute_mse(void);
void compute_C(void);
void compute_SE(void);
void compute_TVAL(void);
void compute_PVAL(void);
void print_B(void);
void print_X(void);
void print_T(void);
void print_G(void);
void print_Z(void);
void regress(void);
void print_parameter_estimates(void);
void print_anova_table(void);
void print_diag_table(void);
void print_diag_table(void);
void proc_step(void);
void run(char *s);
void run1(char *s);
void parse_default(void);
void parse_alpha_option(void);
void parse_data_option(void);
void parse_maxdec_option(void);
void syntax(void);
void * xmalloc(int size);
void * xcalloc(int size);
void * xrealloc(void *p, int size);
void print_pgm(void);
void stop(char *s);
void expected(char *s);
void scan(void);
int scan1(void);
void keyword(void);
char * get_dataline(char *buf, int len);
void get_next_token(void);
double LogGamma(double x);
double Betinc(double X, double A, double B);
double tdist(double X, double df);
double qt(double p, double df);
double Betacdf(double Z, double A, double B);
double fdist(double x, double df1, double df2);
double qf(double p, double df1, double df2);
void title_stmt(void);
void title1_stmt(void);
void title2_stmt(void);
void title3_stmt(void);
