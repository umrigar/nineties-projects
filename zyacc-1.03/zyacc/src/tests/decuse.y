/* Test to ensure declaration of single character identifiers before use
 * in a brace-enclosed block.
 */
%{
#include <ctype.h>
#include <stdio.h>
#include <string.h>

typedef char ID;

#define ENV_SIZE 100
typedef struct {
  unsigned index;
  char env[ENV_SIZE];
} EnvStruct;

typedef EnvStruct *Env;

static Env emptyEnv(void);
static Env addEnv(Env env, ID id);
static int inEnv(ID id, Env env);
void yyerror();
int yylex();

%}

%token <id>(ID $id) ID_TOK;

%%

program
  : decs($env) '{' uses($env) '}'
  ;
decs(Env $envZ)
  : decs($env1)
    ID_TOK($id)
    { $envZ= addEnv($env1, $id); }
  | /* empty */
    { $envZ= emptyEnv(); }
  | error	
    { $envZ= emptyEnv(); }
  ;
uses(%in Env $env)
  : uses($env)
    ID_TOK($id)
    { printf("`%c' %s.\n", $id, 
	     inEnv($id, $env) ? "used" : "not declared"); 
    }
  | /* empty */
  | error
  ;

%%

static EnvStruct env;

static Env emptyEnv(void)
{
  env.index= 0;
  env.env[0]= '\0';
  return &env;
}

static Env addEnv(Env env, ID id)
{
  if (strchr(env->env, id)) {
    printf("Multiple declaration of `%c'.\n", id);
  }
  else {
    env->env[env->index++]= id; env->env[env->index]= '\0';
    printf("`%c' declared.\n", id);
  }
  return env;
}

static int inEnv(ID id, Env env) 
{
  return strchr(env->env, id) ? 1 : 0;
}

int yylex() {
  int c= getchar();
  while (isspace(c)) c= getchar();
  if (c == EOF) return 0;
  else if (c == '{' || c == '}') return c;
  else { yylval.id.id= c; return ID_TOK; }
}

void yyerror(const char *s) {
  printf("%s\n", s);
}

int main()
{
  return yyparse();
}
