%{
/* Performance test on REJECT (no echoing).  */
%}
%%
a    |
ab   |
abc  |
abcd |
b    |
bc   |
bcd  |
c    |
cd   REJECT; 
.|\n ;
