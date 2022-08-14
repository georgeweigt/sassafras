char *alfalfa_str =
"* Alfalfa ;\n"
"* Compare yields of four types of alfalfa. ;\n"
"* This is a randomized complete block design. ;\n"
"* https://archived.stat.ufl.edu/casella/StatDesign/WebDataSets/Alfalfa.txt ;\n"
"\n"
"data ;\n"
"input Variety $ Block $ Rep $ Yield ;\n"
"datalines ;\n"
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
";\n"
"\n"
"proc anova ;\n"
"model Yield = Block Variety Block*Variety ;\n"
"means Variety / lsd ;\n"
;

char *dryweight_str =
"* Dryweight ;\n"
"* Compare dry weights of geraniums grown with three types of fertilizer. ;\n"
"* https://archived.stat.ufl.edu/casella/StatDesign/WebDataSets/DryWeight.txt ;\n"
"\n"
"data ;\n"
"input Fert $ DryWeight ;\n"
"datalines ;\n"
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
";\n"
"\n"
"proc anova ;\n"
"model DryWeight = Fert ;\n"
"means Fert / lsd ;\n"
;

char *fishtank_str =
"* Fishtank ;\n"
"* Compare fish weight gains for three types of diets. ;\n"
"* Tanks are nested within diets. ;\n"
"* https://archived.stat.ufl.edu/casella/StatDesign/WebDataSets/FishTank.txt ;\n"
"\n"
"data ;\n"
"input Diet $ Tank $ WtGain ;\n"
"datalines ;\n"
"1	1	9.759\n"
"1	1	7.399\n"
"1	1	-0.209\n"
"1	1	2.204\n"
"1	1	0.267\n"
"1	1	3.002\n"
"1	2	8.031\n"
"1	2	6.25\n"
"1	2	0.134\n"
"1	2	4.594\n"
"1	2	4.414\n"
"1	2	9.816\n"
"1	3	-2.23\n"
"1	3	9.69\n"
"1	3	-2.239\n"
"1	3	9.499\n"
"1	3	6.927\n"
"1	3	3.449\n"
"1	4	4.274\n"
"1	4	7.52\n"
"1	4	12.141\n"
"1	4	4.828\n"
"1	4	3.391\n"
"1	4	3.621\n"
"2	5	21.819\n"
"2	5	6.503\n"
"2	5	31.596\n"
"2	5	24.633\n"
"2	5	15.73\n"
"2	5	22.231\n"
"2	6	11.672\n"
"2	6	26.479\n"
"2	6	19.784\n"
"2	6	20.884\n"
"2	6	21.811\n"
"2	6	26.344\n"
"2	7	22.161\n"
"2	7	16.429\n"
"2	7	23.311\n"
"2	7	21.983\n"
"2	7	12.181\n"
"2	7	18.252\n"
"2	8	30.865\n"
"2	8	17.875\n"
"2	8	24.562\n"
"2	8	20.442\n"
"2	8	20.791\n"
"2	8	19.44\n"
"3	9	47.588\n"
"3	9	38.219\n"
"3	9	44.445\n"
"3	9	47.115\n"
"3	9	30.83\n"
"3	9	58.708\n"
"3	10	62.477\n"
"3	10	29.834\n"
"3	10	49.902\n"
"3	10	59.946\n"
"3	10	46.587\n"
"3	10	38.567\n"
"3	11	54.545\n"
"3	11	38.846\n"
"3	11	52.163\n"
"3	11	43.141\n"
"3	11	27.822\n"
"3	11	58.025\n"
"3	12	57.749\n"
"3	12	47.737\n"
"3	12	40.653\n"
"3	12	45.344\n"
"3	12	46.702\n"
"3	12	50.091\n"
";\n"
"\n"
"proc anova ;\n"
"model WtGain = Diet Tank*Diet ;\n"
"means Diet / lsd ;\n"
;

char *peanut_str =
"* Peanut ;\n"
"* Compare peanut yields for four treatments. ;\n"
"* This is a latin square design. ;\n"
"* https://archived.stat.ufl.edu/casella/StatDesign/WebDataSets/Peanut.txt ;\n"
"\n"
"data ;\n"
"input Row $ Column $ Treatment $ Yield ;\n"
"datalines ;\n"
"1   1   3   26.7\n"
"2   1   1   23.1\n"
"3   1   2   28.3\n"
"4   1   4   25.1\n"
"1   2   1   19.7\n"
"2   2   2   20.7\n"
"3   2   4   20.1\n"
"4   2   3   17.4\n"
"1   3   2   28\n"
"2   3   4   24.9\n"
"3   3   3   29\n"
"4   3   1   28.7\n"
"1   4   4   29.8\n"
"2   4   3   29\n"
"3   4   1   27.3\n"
"4   4   2   34.1\n"
";\n"
"\n"
"proc anova ;\n"
"model Yield = Row Column Treatment ;\n"
"means Treatment / lsd ;\n"
;

char *redclover_str =
"* Redclover ;\n"
"* Compare red clover yields for two levels of nitrogen and four levels of sulphur. ;\n"
"* This is a two-way completely randomized design. ;\n"
"* https://archived.stat.ufl.edu/casella/StatDesign/WebDataSets/RedClover.txt ;\n"
"\n"
"data ;\n"
"input Nitrogen $ Sulphur $ Yield ;\n"
"datalines ;\n"
"0    0   4.48\n"
"0    0   4.52\n"
"0    0   4.63\n"
"0    3   4.7\n"
"0    3   4.65\n"
"0    3   4.57\n"
"0    6   5.21\n"
"0    6   5.23\n"
"0    6   5.28\n"
"0    9   5.88\n"
"0    9   5.98\n"
"0    9   5.88\n"
"20   0   5.76\n"
"20   0   5.72\n"
"20   0   5.78\n"
"20   3   7.01\n"
"20   3   7.11\n"
"20   3   7.02\n"
"20   6   5.88\n"
"20   6   5.82\n"
"20   6   5.73\n"
"20   9   6.26\n"
"20   9   6.26\n"
"20   9   6.37\n"
";\n"
"\n"
"proc anova ;\n"
"model Yield = Sulphur Nitrogen Sulphur*Nitrogen ;\n"
"means Sulphur*Nitrogen ;\n"
;

char *strawberry_str =
"* Strawberry ;\n"
"* Compare yields for three types of strawberries. ;\n"
"* This is a randomized complete block design. ;\n"
"* https://archived.stat.ufl.edu/casella/StatDesign/WebDataSets/Strawberry.txt ;\n"
"\n"
"data ;\n"
"input Block $ Treatment $ Yield ;\n"
"datalines ;\n"
"1   A   10.1\n"
"1   B   6.3\n"
"1   C   8.4\n"
"2   A   10.8\n"
"2   B   6.9\n"
"2   C   9.4\n"
"3   A   9.8\n"
"3   B   5.3\n"
"3   C   9\n"
"4   A   10.5\n"
"4   B   6.2\n"
"4   C   9.2\n"
";\n"
"\n"
"proc anova ;\n"
"model Yield = Block Treatment ;\n"
"means Treatment / lsd ;\n"
;

char *trees_str =
"* Trees ;\n"
"\n"
"data ;\n"
"input Girth Height Volume ;\n"
"LG = log(Girth) ;\n"
"LH = log(Height) ;\n"
"LV = log(Volume) ;\n"
"datalines ;\n"
"  8.3     70   10.3\n"
"  8.6     65   10.3\n"
"  8.8     63   10.2\n"
" 10.5     72   16.4\n"
" 10.7     81   18.8\n"
" 10.8     83   19.7\n"
" 11.0     66   15.6\n"
" 11.0     75   18.2\n"
" 11.1     80   22.6\n"
" 11.2     75   19.9\n"
" 11.3     79   24.2\n"
" 11.4     76   21.0\n"
" 11.4     76   21.4\n"
" 11.7     69   21.3\n"
" 12.0     75   19.1\n"
" 12.9     74   22.2\n"
" 12.9     85   33.8\n"
" 13.3     86   27.4\n"
" 13.7     71   25.7\n"
" 13.8     64   24.9\n"
" 14.0     78   34.5\n"
" 14.2     80   31.7\n"
" 14.5     74   36.3\n"
" 16.0     72   38.3\n"
" 16.3     77   42.6\n"
" 17.3     81   55.4\n"
" 17.5     82   55.7\n"
" 17.9     80   58.3\n"
" 18.0     80   51.5\n"
" 18.0     80   51.0\n"
" 20.6     87   77.0\n"
";\n"
"\n"
"proc reg ;\n"
"model LV = LG LH ;\n"
;
