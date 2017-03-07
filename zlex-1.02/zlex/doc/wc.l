/* Word-count program for stdin. */
%%
%{
  unsigned cc= 0;       /* # of chars seen so far. */
  unsigned wc= 0;       /* # of words seen so far. */
  unsigned lc= 0;       /* # of lines seen so far. */
%}


[^ \t\n]+	wc++; cc+= yyleng; 
[ \t]+          cc+= yyleng;
\n+             lc+= yyleng; cc+= yyleng;
<<EOF>>         printf("%d %d %d\n", lc, wc, cc);

