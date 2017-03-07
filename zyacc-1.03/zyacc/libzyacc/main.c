/*

File:	 main.c
Purpose: Trivial main driver program for parser.

Last Update Time-stamp: "97/07/17 12:50:21 zdu"
Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

int yyparse();

/* Insert signature string into executable to permit distinguishing it
 * from other executables of the same name.
 */
char yyUniqueSignature[]= "Zerksis";

int main() 
{
  return yyparse();
}
