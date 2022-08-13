char *alfalfa_str =
"* Alfalfa\n"
"data\n"
"input Variety$ Block$ Rep$ Yield\n"
"datalines\n"
"Ladak   1   1   3.1\n"
"Ladak   2   1   4.06\n"
"Ladak   3   1   4.73\n"
"Ladak   4   1   3.1\n"
"Ladak   1   2   3.25\n"
"Ladak   2   2   4.26\n"
"Ladak   3   2   4.71\n"
"Ladak   4   2   4.21\n"
"Ladak   1   3   3.86\n"
"Ladak   2   3   4.53\n"
"Ladak   3   3   5.26\n"
"Ladak   4   3   3.84\n"
"Narrag  1   1   4.65\n"
"Narrag  2   1   5.64\n"
"Narrag  3   1   4.94\n"
"Narrag  4   1   5.38\n"
"Narrag  1   2   5.46\n"
"Narrag  2   2   5.48\n"
"Narrag  3   2   5.26\n"
"Narrag  4   2   5.68\n"
"Narrag  1   3   4.21\n"
"Narrag  2   3   5.09\n"
"Narrag  3   3   5.8\n"
"Narrag  4   3   5.82\n"
"DuPuits 1   1   5.47\n"
"DuPuits 2   1   5.62\n"
"DuPuits 3   1   6.71\n"
"DuPuits 4   1   6.87\n"
"DuPuits 1   2   6.41\n"
"DuPuits 2   2   6.3\n"
"DuPuits 3   2   6.96\n"
"DuPuits 4   2   6.28\n"
"DuPuits 1   3   5.57\n"
"DuPuits 2   3   6.46\n"
"DuPuits 3   3   5.92\n"
"DuPuits 4   3   6.46\n"
"Flamand 1   1   6.85\n"
"Flamand 2   1   6.33\n"
"Flamand 3   1   6.88\n"
"Flamand 4   1   6.23\n"
"Flamand 1   2   6.34\n"
"Flamand 2   2   5.83\n"
"Flamand 3   2   6.59\n"
"Flamand 4   2   6.52\n"
"Flamand 1   3   5.45\n"
"Flamand 2   3   4.33\n"
"Flamand 3   3   6.06\n"
"Flamand 4   3   6.81\n"
"\n"
"proc anova\n"
"model Yield = Block Variety Block*Variety\n"
"means Variety / lsd\n";

char *dry_weight_str =
"* Dry Weight\n"
"data\n"
"input Fert$ DryWeight\n"
"datalines\n"
"A	1.02\n"
"A	0.79\n"
"A	1\n"
"A	0.59\n"
"A	0.97\n"
"B	1\n"
"B	1.21\n"
"B	1.22\n"
"B	0.96\n"
"B	0.79\n"
"C	0.99\n"
"C	1.36\n"
"C	1.22\n"
"C	1.12\n"
"C	1.17\n"
"\n"
"proc anova\n"
"model DryWeight = Fert\n"
"means Fert / lsd\n";
