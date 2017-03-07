/* Test for intra-token patterns. */
%option -i
%%
%{
  int c1= 0, c2= 0, c3= 0, c4= 0, c5= 0, c6= 0;
%}
abx	c1++;
abr  	c2++;
+abcf	c3++;
+abx	c4++;
+abr  	c5++;
.|\n	c6++;
<<EOF>> printf("c1= %d; c2= %d; c3= %d; c4= %d; c5= %d; c6= %d\n", 
	       c1, c2, c3, c4, c5, c6);


