#######  Output of LSystem editor program  ########
#
#
# recursion levels
4-4-3
# default angle
45.000
# default thickness
0.400
# default lenght
1.000
# PointSprites Sizes
11.000-4.900-2.000-0.700
#
# axiom:
FFAL
# rules:
A(70)=F[&'(.9)!B]>(137)'(.9)!A
A(90)=F[&(30)'(.9)!B]>(78)'(.9)!A
A(100)=F[&(75)'(.9)!B]>(100)'(.9)!A
B(50)=F[-'(.9)$<(15)!C]L'(.9)!C
B(70)=F[-(30)$>(40)'(.9)!C]L'(.9)!C
B(100)=F[-(70)$>(23)'(.9)!C]L'(.9)!C
C(50)=F[+'(.9)!$>(24)B]L'(.9)!B
C(70)=F[+(30)'(.9)!$<(54)B]L'(.9)!B
C(100)=F[+(70)'(.9)!$B]L'(.9)!B
@