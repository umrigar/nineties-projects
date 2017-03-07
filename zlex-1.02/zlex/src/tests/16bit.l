/* Test for 16-bit scanner. */
%option --16-bit

a	[\x100-\x1ff]
b	[\x1000-\x10ff]
c	[\x1080-\x1088]
d	[\x1101-\x1120]
z	[\x00]

%%
{a}+{b}		printf("1: %d\n", yyleng);
{b}+{z}{b}+	printf("2: %d\n", yyleng);
{c}+		printf("3: %d\n", yyleng);
{d}+{a}		printf("4: %d\n", yyleng);
.|\n
%%

int main() {
  int t;
  while ((t= yylex())) {
    printf("%d\n", t);
  }
  return 0;
}
