void data_step(void);
void parse_data_body(void);
void read_data_file(void);
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
double incbeta(double a, double b, double x);
int main(int argc, char *argv[]);
void emit_line_init(void);
void emit_line(char *s);
void emit_line_center(char *s);
void parse_by_stmt(void);
void parse_class_stmt(void);
void parse_comment_stmt(void);
void parse_var_stmt(void);
void print_table_and_free(char **a, int nrow, int ncol, char *fmt);
void print_table(char **a, int nrow, int ncol, char *fmt);
void print_title(void);
void proc_anova(void);
void proc_anova_parse_stmt(void);
void proc_anova_parse_body(void);
void proc_anova_parse_means_stmt(void);
void proc_anova_parse_means_item(void);
void proc_anova_parse_model_stmt(void);
void proc_anova_parse_model_options(void);
void proc_anova_parse_explanatory_variable(void);
int proc_anova_get_var_index(void);
void proc_anova_add_interaction(int n);
void proc_anova_create_interaction_level_names(int n);
void proc_anova_add_factorial(int n);
void proc_anova_add_nested(int n);
void proc_anova_model(void);
void proc_anova_regression(void);
void proc_anova_prelim(void);
void proc_anova_fit(void);
void proc_anova_fit1(int x, int level);
int proc_anova_compute_G(void);
void proc_anova_compute_B(void);
void proc_anova_compute_Yhat(void);
void proc_anova_compute_ss(void);
void print_anova_table_part1(void);
void print_anova_table_part2(void);
void print_anova_table_part3(void);
void proc_anova_compute_means(int x);
void proc_anova_print_means(int x);
void proc_anova_print_lsd(int x);
void proc_anova_print_ttest(int x);
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
void proc_reg_parse_stmt(void);
void proc_reg_parse_body(void);
void proc_reg_parse_model_stmt(void);
void proc_reg_parse_model_options(void);
void proc_reg_compute_X(void);
void proc_reg_compute_T(void);
int proc_reg_compute_G(void);
void proc_reg_compute_B(void);
void proc_reg_compute_Yhat(void);
void proc_reg_compute_mse(void);
void proc_reg_compute_C(void);
void proc_reg_compute_SE(void);
void proc_reg_compute_TVAL(void);
void proc_reg_compute_PVAL(void);
void proc_reg_print_B(void);
void proc_reg_print_X(void);
void proc_reg_print_T(void);
void proc_reg_print_G(void);
void proc_reg_print_Z(void);
void proc_reg_regress(void);
void proc_reg_print_parameter_estimates(void);
void proc_reg_print_anova_table(void);
void proc_reg_print_diag_table(void);
void proc_step(void);
char * read_file(char *filename);
void run(char *s);
void run_nib(char *s);
void parse_default(void);
void parse_alpha_option(void);
void parse_data_option(void);
void parse_maxdec_option(void);
void * xmalloc(int size);
void * xrealloc(void *p, int size);
void xfree(void *p);
void print_pgm(void);
void stop(char *s);
void expected(char *s);
void scan(void);
int scan1(void);
void keyword(void);
char * get_dataline(char *buf, int len);
void get_next_token(void);
double tdist(double t, double df);
double qt(double p, double df);
double fdist(double t, double df1, double df2);
void title_stmt(void);
void title1_stmt(void);
void title2_stmt(void);
void title3_stmt(void);
