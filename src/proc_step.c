void
proc_step(void)
{
	nby = 0;
	nvar = 0;
	nstats = 0;
	nclass = 0;

	maxdec = 3;
	alpha = 0.05;

	select_dataset(NULL);

	scan(); // get token after PROC

	keyword();

	switch (token) {
	case KANOVA:
		proc_anova();
		break;
	case KMEANS:
		proc_means();
		break;
	case KPRINT:
		proc_print();
		break;
	case KREG:
		proc_reg();
		break;
	default:
		expected("procedure name");
	}
}
