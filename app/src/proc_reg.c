#include "defs.h"

extern int num_x;
extern int xtab[];

extern int num_y;
extern int ytab[];

static void parse_proc_reg_stmt(void);
static void parse_proc_reg_body(void);

void
proc_reg()
{
	num_x = 0;
	num_y = 0;

	parse_proc_reg_stmt();

	if (dataset == NULL)
		stop("No data set");

	parse_proc_reg_body();

	regress();

	print_title();

	emit_line_center("Analysis of Variance");
	emit_line("");

	print_anova_table();
	print_diag_table();

	emit_line_center("Parameter Estimates");
	emit_line("");

	print_parameter_estimates();
}

static void
parse_proc_reg_stmt()
{
	for (;;) {
		scan();
		keyword();
		switch (token) {
		case ';':
			scan(); // eat the semicolon
			return;
		case KALPHA:
			parse_alpha_option();
			break;
		case KDATA:
			parse_data_option();
			break;
		default:
			expected("proc reg option");
		}
	}
}

static void
parse_proc_reg_body()
{
	for (;;) {
		keyword();
		switch (token) {
		case 0:
		case KDATA:
		case KPROC:
		case KRUN:
			return;
		case KMODEL:
			model_stmt();
			break;
		default:
			parse_default();
			break;
		}
	}
}
