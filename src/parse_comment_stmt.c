void
parse_comment_stmt(void)
{
	while (*inp && *inp != ';')
		inp++;
	scan();
}
