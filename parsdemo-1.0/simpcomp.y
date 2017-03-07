%token 'mod'
%token 'div'
%token ID
%token NUM
%token ':='=ASSGN
%%
program
  : stmts
  ;
stmts
  : assgnStmt
  | assgnStmt ';' stmts
  ;  
assgnStmt
  : ID ':='  expr
  ;
expr
  : expr '+' term
  | expr '-' term
  | term
  ;
term
  : term '*' factor
  | term '/' factor
  | term 'div' factor
  | term 'mod' factor
  | factor
  ;
factor
  : '(' expr ')'
  | ID
  | NUM
  ;
