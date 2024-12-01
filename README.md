[Sassafras Manual](https://georgeweigt.github.io/sassafras.pdf)

#

Build and run for Linux

```
make
./sassafras filename
```

Example

```
./sassafras demos/corrosion-demo

                              Analysis of Variance

   Source     DF     Sum of Squares       Mean Square     F Value     Pr > F
   Model      14     48517.75000000     3465.55357143       27.83     0.0000
   Error       9      1120.87500000      124.54166667                       
   Total      23     49638.62500000                                         

              R-Square     Coeff Var      Root MSE         Y Mean
              0.977419     11.035672     11.159824     101.125000

   Source     DF           Anova SS        Mean Square     F Value     Pr > F
   T           2     26519.25000000     13259.62500000      106.47     0.0000
   C           3      4289.12500000      1429.70833333       11.48     0.0020
   T*C         6      3269.75000000       544.95833333        4.38     0.0241
   W           3     14439.62500000      4813.20833333       38.65     0.0000

                                 Mean Response

             C      N         Mean Y     95% CI MIN     95% CI MAX
             C2     6      90.166667      79.860326     100.473007
             C3     6      95.666667      85.360326     105.973007
             C1     6      94.666667      84.360326     104.973007
             C4     6     124.000000     113.693660     134.306340

                       Least Significant Difference Test

  C     C        Delta Y    95% CI MIN    95% CI MAX    t Value    Pr > |t|  
  C2    C3     -5.500000    -20.075366      9.075366      -0.85      0.4155  
  C2    C1     -4.500000    -19.075366     10.075366      -0.70      0.5026  
  C2    C4    -33.833333    -48.408700    -19.257967      -5.25      0.0005 *
  C3    C2      5.500000     -9.075366     20.075366       0.85      0.4155  
  C3    C1      1.000000    -13.575366     15.575366       0.16      0.8801  
  C3    C4    -28.333333    -42.908700    -13.757967      -4.40      0.0017 *
  C1    C2      4.500000    -10.075366     19.075366       0.70      0.5026  
  C1    C3     -1.000000    -15.575366     13.575366      -0.16      0.8801  
  C1    C4    -29.333333    -43.908700    -14.757967      -4.55      0.0014 *
  C4    C2     33.833333     19.257967     48.408700       5.25      0.0005 *
  C4    C3     28.333333     13.757967     42.908700       4.40      0.0017 *
  C4    C1     29.333333     14.757967     43.908700       4.55      0.0014 *
```

#

To build for macOS (requires Xcode)

```
cd xcode
make
mv build/Release/Sassafras.app ~/Desktop
```
