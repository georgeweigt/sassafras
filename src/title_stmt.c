void
title_stmt(void)
{
	scan();
	if (token != STRING)
		expected("string");
	if (title)
		free(title);
	if (strbuf[0] == 0)
		title = NULL;
	else
		title = strdup(strbuf);
	scan();
	if (token != ';')
		stop("';' expected");
	scan();
}

void
title1_stmt(void)
{
	scan();
	if (token != STRING)
		expected("string");
	if (title1)
		free(title1);
	if (strbuf[0] == 0)
		title1 = NULL;
	else
		title1 = strdup(strbuf);
	scan();
	if (token != ';')
		stop("';' expected");
	scan();
}

void
title2_stmt(void)
{
	scan();
	if (token != STRING)
		expected("string");
	if (title2)
		free(title2);
	if (strbuf[0] == 0)
		title2 = NULL;
	else
		title2 = strdup(strbuf);
	scan();
	if (token != ';')
		stop("';' expected");
	scan();
}

void
title3_stmt(void)
{
	scan();
	if (token != STRING)
		expected("string");
	if (title3)
		free(title3);
	if (strbuf[0] == 0)
		title3 = NULL;
	else
		title3 = strdup(strbuf);
	scan();
	if (token != ';')
		stop("';' expected");
	scan();
}
