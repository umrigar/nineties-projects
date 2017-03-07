/*

File:	 parse.y
Purpose: ANSI-C parser. Outputs source with profiling counters added.

Last Update Time-stamp: <97/08/01 08:02:37 zdu>

Copyright (C) 1995, 1997 Zerksis D. Umrigar

See the file LICENSE for copying and distribution conditions.
THERE IS ABSOLUTELY NO WARRANTY FOR THIS PROGRAM.

*/

%{

#include "options.h"
#include "out.h"
#include "scan.h"

#include "area.h"
#include "error.h"
#include "hashtab.h"
#include "ids.h"
#include "parseopt.h"

#define YY_LEX scan

static void dclTypedef PROTO((Index id, BooleanX isTypedef));
static Boolean isTypedef PROTO((VOID_ARGS));
static VOID beginScope PROTO((VOID_ARGS));
static VOID endScope PROTO((VOID_ARGS));
static Int helpOptFn 
  PROTO((Int id, VOIDP checkP, VOIDP valP, ConstString argP));
static Int typedefOptFn 
  PROTO((Int id, VOIDP checkP, VOIDP valP, ConstString argP));
static Int versionOptFn 
  PROTO((Int id, VOIDP checkP, VOIDP valP, ConstString argP));


static struct {
  Boolean seenType;
} globs;
#define G globs

#define OUT(t)		putOut(&t)

enum {
  TYPEDEF_DCL,		/* declaration of a typedef */
  ID_DCL,		/* declaration of other ID in typedef namespace */
  OTHER_DCL,		/* declaration of other ID in other namespace */
};

#if YY_ZYACC_DEBUG

static YY_VOID tokSemFn PROTO((FILE *out, int tokNum, VOIDP p));

#define YY_SEM_FN tokSemFn

#endif /* if YY_ZYACC_DEBUG */

%}

%token <tok>(TokSem $t)			AUTO_TOK = 'auto'
%token <tok>(TokSem $t)			BREAK_TOK = 'break'
%token <tok>(TokSem $t)			CASE_TOK = 'case'
%token <tok>(TokSem $t)			CHAR_TOK = 'char'
%token <tok>(TokSem $t)			CONST_TOK = 'const'
%token <tok>(TokSem $t)			CONTINUE_TOK = 'continue'
%token <tok>(TokSem $t)			DEFAULT_TOK = 'default'
%token <tok>(TokSem $t)			DO_TOK = 'do'
%token <tok>(TokSem $t)			DOUBLE_TOK = 'double'
%token <tok>(TokSem $t)			ELSE_TOK = 'else'
%token <tok>(TokSem $t)			ENUM_TOK = 'enum'
%token <tok>(TokSem $t)			EXTERN_TOK = 'extern'
%token <tok>(TokSem $t)			FLOAT_TOK = 'float'
%token <tok>(TokSem $t)			FOR_TOK = 'for'
%token <tok>(TokSem $t)			GOTO_TOK = 'goto'
%token <tok>(TokSem $t)			IF_TOK = 'if'
%token <tok>(TokSem $t)			INT_TOK = 'int'
%token <tok>(TokSem $t)			LONG_TOK = 'long'
%token <tok>(TokSem $t)			REGISTER_TOK = 'register'
%token <tok>(TokSem $t)			RETURN_TOK = 'return'
%token <tok>(TokSem $t)			SHORT_TOK = 'short'
%token <tok>(TokSem $t)			SIGNED_TOK = 'signed'
%token <tok>(TokSem $t)			SIZEOF_TOK = 'sizeof'
%token <tok>(TokSem $t)			STATIC_TOK = 'static'
%token <tok>(TokSem $t)			STRUCT_TOK = 'struct'
%token <tok>(TokSem $t)			SWITCH_TOK = 'switch'
%token <tok>(TokSem $t)			TYPEDEF_TOK = 'typedef'
%token <tok>(TokSem $t)			UNION_TOK = 'union'
%token <tok>(TokSem $t)			UNSIGNED_TOK = 'unsigned'
%token <tok>(TokSem $t)			VOID_TOK = 'void'
%token <tok>(TokSem $t)			VOLATILE_TOK = 'volatile'
%token <tok>(TokSem $t)			WHILE_TOK = 'while'

%token <tok>(TokSem $t)			ADD_ASSGN_TOK = '+='
%token <tok>(TokSem $t)			AND_ASSGN_TOK = '&='
%token <tok>(TokSem $t)			BOOL_AND_TOK = '&&'
%token <tok>(TokSem $t)			BOOL_OR_TOK = '||'
%token <tok>(TokSem $t)			DEC_TOK = '--'  
%token <tok>(TokSem $t)			DEREF_TOK = '->'
%token <tok>(TokSem $t)			DIV_ASSGN_TOK = '/='
%token <tok>(TokSem $t)			DOT_DOT_TOK = '...'
%token <tok>(TokSem $t)			EQ_TOK = '=='   
%token <tok>(TokSem $t)			GE_TOK = '>='    
%token <tok>(TokSem $t)			INC_TOK = '++'
%token <tok>(TokSem $t)			LE_TOK = '<='  
%token <tok>(TokSem $t)			LSH_ASSGN_TOK = '<<='
%token <tok>(TokSem $t)			LSH_TOK = '<<'  
%token <tok>(TokSem $t)			MOD_ASSGN_TOK = '%='
%token <tok>(TokSem $t)			MULT_ASSGN_TOK = '*='
%token <tok>(TokSem $t)			NE_TOK = '!='   
%token <tok>(TokSem $t)			OR_ASSGN_TOK = '|='
%token <tok>(TokSem $t)			RSH_ASSGN_TOK = '>>='
%token <tok>(TokSem $t)			RSH_TOK = '>>'  
%token <tok>(TokSem $t)			SUB_ASSGN_TOK = '-='
%token <tok>(TokSem $t)			XOR_ASSGN_TOK = '^='

%token	<tok>(TokSem $t)		';' '{' '}' '[' ']' '(' ')' ',' ':'
%token	<tok>(TokSem $t)		'=' '*' '?' '|' '^' '&' '<' '>'
%token	<tok>(TokSem $t)		'+' '-' '/' '%' '~' '!' '.'

%token	<tok>(TokSem $t)		ID_TOK
%token	<tok>(TokSem $t)		NUM_TOK
%token	<tok>(TokSem $t)		STRING_TOK

%start translation_unit

%%

/*

NOTE: Most all-upper-case identifiers are actually nonterminals (tho' only
1-level removed from terminals).

*/

/*			    DECLARATIONS				*/

translation_unit
  : external_declaration
  | translation_unit external_declaration
  ;

external_declaration
  : function_definition
  | declaration
  | error
  ;

function_definition
  : fnDeclarator compound_statement(FN_COUNTER, $counter)
  | fnDeclarator declaration_list compound_statement(FN_COUNTER, $counter)
  | declaration_specifiers($dclType) declarator($dclType) 
    compound_statement(FN_COUNTER, $counter)
  | declaration_specifiers($dclType) declarator($dclType) declaration_list 
    compound_statement(FN_COUNTER, $counter)
  ;

fnDeclarator
  : declarator(ID_DCL)
  ;

declaration
  : declaration_specifiers($dclType) SEMI
  | declaration_specifiers($dclType) init_declarator_list($dclType) SEMI
  ;

declaration_list
  : declaration
  | declaration_list declaration
  ;

declaration_specifiers(Index $dclType)
  : declaration_specifiers_x($dclType)
	{ G.seenType= FALSE; }
  ;

declaration_specifiers_x(Index $dclType)
  : storage_class_specifier($dclType)
  | storage_class_specifier($dclType1) 
    declaration_specifiers_x($dclType2)
	{ $dclType= ($dclType1 == TYPEDEF_DCL || $dclType2 == TYPEDEF_DCL)
                    ? TYPEDEF_DCL
                    : ID_DCL; 
	}
  | type_specifier
	{ $dclType= ID_DCL; }
  | type_specifier declaration_specifiers_x($dclType)
  | type_qualifier
	{ $dclType= ID_DCL; }
  | type_qualifier declaration_specifiers_x($dclType)
  ;

storage_class_specifier(Index $dclType)
  : TYPEDEF
	{ $dclType= TYPEDEF_DCL; }
  | EXTERN
	{ $dclType= ID_DCL; }
  | STATIC
	{ $dclType= ID_DCL; }
  | AUTO
	{ $dclType= ID_DCL; }
  | REGISTER
	{ $dclType= ID_DCL; }
  ;

type_specifier
  : CHAR
	{ G.seenType= TRUE; }
  | SHORT
	{ G.seenType= TRUE; }
  | INT
	{ G.seenType= TRUE; }
  | LONG
	{ G.seenType= TRUE; }
  | SIGNED
	{ G.seenType= TRUE; }
  | UNSIGNED
	{ G.seenType= TRUE; }
  | FLOAT
	{ G.seenType= TRUE; }
  | DOUBLE
	{ G.seenType= TRUE; }
  | VOID
	{ G.seenType= TRUE; }
  | struct_or_union_specifier
	{ G.seenType= TRUE; }
  | enum_specifier
	{ G.seenType= TRUE; }
  | typedef_name
	{ G.seenType= TRUE; }
  ;

type_qualifier
  : CONST
  | VOLATILE
  ;

struct_or_union_specifier
  : struct_or_union ID($id) LBRACE struct_declaration_list RBRACE
  | struct_or_union LBRACE struct_declaration_list RBRACE
  | struct_or_union ID($id)
  ;

struct_or_union
  : STRUCT
  | UNION
  ;

struct_declaration_list
  : struct_declaration
  | struct_declaration_list struct_declaration
  ;

init_declarator_list(%in Index $dclType)
  : init_declarator($dclType)
  | init_declarator_list($dclType) COMMA init_declarator($dclType)
  ;

init_declarator(%in Index $dclType)
  : declarator($dclType)
  | declarator($dclType) EQ initializer
  ;

struct_declaration
  : specifier_qualifier_list struct_declarator_list SEMI
  ;

specifier_qualifier_list
  : specifier_qualifier_list_x
	{ G.seenType= FALSE; }
  ;

specifier_qualifier_list_x
  : type_specifier
  | type_specifier specifier_qualifier_list_x
  | type_qualifier
  | type_qualifier specifier_qualifier_list_x
  ;

struct_declarator_list
  : struct_declarator
  | struct_declarator_list COMMA struct_declarator
  ;

struct_declarator
  : fieldDeclarator
  | COLON constant_expr
  | fieldDeclarator COLON constant_expr
  ;

fieldDeclarator
  : declarator(OTHER_DCL)
  ;

enum_specifier
  : ENUM LBRACE enumerator_list RBRACE
  | ENUM ID($id) LBRACE enumerator_list RBRACE
  | ENUM ID($id)
  ;

enumerator_list
  : enumerator
  | enumerator_list COMMA enumerator
  ;

enumerator
  : ID($id)
  | ID($id) EQ constant_expr
  ;

declarator(%in Index $dclType)
  : direct_declarator($dclType)
  | pointer direct_declarator($dclType)
  ;

direct_declarator(%in Index $dclType)
  : ID($id)
	{ if ($dclType != OTHER_DCL) dclTypedef($id, $dclType == TYPEDEF_DCL);
	}
  | LPAREN declarator($dclType) RPAREN
  | direct_declarator($dclType) LBRACKET RBRACKET
  | direct_declarator($dclType) LBRACKET constant_expr RBRACKET
  | direct_declarator($dclType) LPAREN parameter_type_list RPAREN
  | direct_declarator($dclType) LPAREN RPAREN
  | direct_declarator($dclType) LPAREN identifier_list RPAREN
  ;

pointer
  : STAR
  | STAR type_qualifier_list
  | STAR pointer
  | STAR type_qualifier_list pointer
  ;

type_qualifier_list
  : type_qualifier
  | type_qualifier_list type_qualifier
  ;

parameter_type_list
  : parameter_list
  | parameter_list COMMA DOT_DOT_DOT
  ;

parameter_list
  : parameter_declaration
  | parameter_list COMMA parameter_declaration
  ;


parameter_declaration
  : declaration_specifiers($dclType) declarator($dclType)
  | declaration_specifiers($dclType)
  | declaration_specifiers($dclType) abstract_declarator
  ;

identifier_list
  : ID($id)
  | identifier_list COMMA ID($id)
  ;

initializer
  : assignment_expr
  | LBRACE initializer_list RBRACE
  | LBRACE initializer_list COMMA RBRACE
  ;

initializer_list
  : initializer
  | initializer_list COMMA initializer
  ;

type_name
  : specifier_qualifier_list
  | specifier_qualifier_list abstract_declarator
  ;

abstract_declarator
  : pointer
  | direct_abstract_declarator
  | pointer direct_abstract_declarator
  ;

direct_abstract_declarator
  : LPAREN abstract_declarator RPAREN
  | LBRACKET RBRACKET
  | LBRACKET constant_expr RBRACKET
  | direct_abstract_declarator LBRACKET RBRACKET
  | direct_abstract_declarator LBRACKET constant_expr RBRACKET
  | LPAREN RPAREN
  | LPAREN parameter_type_list RPAREN
  | direct_abstract_declarator LPAREN RPAREN
  | direct_abstract_declarator LPAREN parameter_type_list RPAREN
  ;

typedef_name
  : %test(!G.seenType && isTypedef()) ID($id) 
       { 
#if  TEST_TYPEDEF
         printf("*"); 
#endif
       }
  ;


/*				STATEMENTS				*/

statement(%in CounterType $counter0, %out CounterType $counterZ)
  : counter_begin($counter0) labeled_statement($counterZ)
    counter_end($counter0) 
  | counter_begin($counter0) expression_statement counter_end($counter0) 
        { $counterZ= NO_COUNTER; }
  | compound_statement(
      $counter0 == STMT_COUNTER ? COMPOUND_COUNTER : $counter0, $counterZ)
  | counter_begin($counter0) selection_statement($counterX)
    counter_end($counter0) 
        { $counterZ= STMT_COUNTER; }
  | counter_begin($counter0) iteration_statement($counterX)
    counter_end($counter0) 
        { $counterZ= STMT_COUNTER; }
  | counter_begin($counter0) jump_statement counter_end($counter0) 
        { $counterZ= NO_COUNTER; }
  ;

labeled_statement(%out CounterType $counterZ)
  : ID($id) COLON statement(STMT_COUNTER, $counterZ)
  | CASE constant_expr COLON statement(STMT_COUNTER, $counterZ)
  | DEFAULT COLON statement(STMT_COUNTER, $counterZ)
  ;

expression_statement
  : SEMI
  | expr SEMI
  ;

compound_statement(%in CounterType $counter0, %out CounterType $counterZ)
  : lbrace counter_begin($counter0) rbrace
	{ $counterZ= NO_COUNTER; }
  | lbrace statement_list($counter0, $counterZ) rbrace
  | lbrace declaration_list rbrace
	{ $counterZ= NO_COUNTER; }
  | lbrace
    declaration_list statement_list($counter0, $counterZ) 
    rbrace
  ;

lbrace
  : LBRACE
	{ beginScope(); }
  ;

rbrace
  : RBRACE
	{ endScope(); }
  ;

statement_list(%in CounterType $counter0, %out CounterType $counterZ)
  : statement($counter0, $counterZ)
  | statement_list($counter0, $counterX) 
    statement($counterX, $counterZ)
  ;

selection_statement(%out CounterType $counterZ)
  : IF LPAREN expr RPAREN needs_counter_statement($counterZ)
  | IF LPAREN expr RPAREN needs_counter_statement($counterT) 
    ELSE statement(STMT_COUNTER, $counterE) 
	{ $counterZ= ($counterT == NO_COUNTER) ? $counterE : $counterT; }
  | SWITCH LPAREN expr RPAREN statement(NO_COUNTER, $counterZ)
  ;

needs_counter_statement(%out CounterType $counterZ)
  : statement(STMT_COUNTER, $counterZ)
  ;

iteration_statement(%out CounterType $counterZ)
  : WHILE LPAREN expr RPAREN statement(STMT_COUNTER, $counterZ)
  | DO statement(STMT_COUNTER, $counterZ) WHILE LPAREN expr RPAREN SEMI
  | FOR LPAREN SEMI SEMI RPAREN statement(STMT_COUNTER, $counterZ)
  | FOR LPAREN SEMI SEMI expr RPAREN statement(STMT_COUNTER, $counterZ)
  | FOR LPAREN SEMI expr SEMI RPAREN statement(STMT_COUNTER, $counterZ)
  | FOR LPAREN SEMI expr SEMI expr RPAREN statement(STMT_COUNTER, $counterZ)
  | FOR LPAREN expr SEMI SEMI RPAREN statement(STMT_COUNTER, $counterZ)
  | FOR LPAREN expr SEMI SEMI expr RPAREN statement(STMT_COUNTER, $counterZ)
  | FOR LPAREN expr SEMI expr SEMI RPAREN statement(STMT_COUNTER, $counterZ)
  | FOR LPAREN expr SEMI expr SEMI expr RPAREN 
    statement(STMT_COUNTER, $counterZ)
  ;

jump_statement
  : GOTO ID($id) SEMI
  | CONTINUE SEMI
  | BREAK SEMI
  | RETURN SEMI
  | RETURN expr SEMI
  ;


/*				EXPRESSIONS				*/

expr
  : assignment_expr
  | expr COMMA assignment_expr
  | error
  ;

assignment_expr
  : conditional_expr
  | unary_expr assignment_operator assignment_expr
  ;

assignment_operator
  : EQ
  | STAR_EQ
  | SLASH_EQ
  | PERCENT_EQ
  | PLUS_EQ
  | MINUS_EQ
  | LT_LT_EQ
  | GT_GT_EQ
  | AMPERSAND_EQ
  | CARAT_EQ
  | BAR_EQ
  ;

conditional_expr
  : logical_or_expr
  | logical_or_expr 
    QUESTION counter_begin(COND_COUNTER) logical_or_expr 
    counter_end(COND_COUNTER)
    COLON counter_begin(COND_COUNTER) 
    conditional_expr counter_end(COND_COUNTER)
  ;

constant_expr
  : conditional_expr
  ;

logical_or_expr
  : logical_and_expr
  | logical_or_expr BAR_BAR logical_and_expr
  ;

logical_and_expr
  : inclusive_or_expr
  | logical_and_expr AMPERSAND_AMPERSAND inclusive_or_expr
  ;

inclusive_or_expr
  : exclusive_or_expr
  | inclusive_or_expr BAR exclusive_or_expr
  ;

exclusive_or_expr
  : and_expr
  | exclusive_or_expr CARAT and_expr
  ;

and_expr
  : equality_expr
  | and_expr AMPERSAND equality_expr
  ;

equality_expr
  : relational_expr
  | equality_expr EQ_EQ relational_expr
  | equality_expr BANG_EQ relational_expr
  ;

relational_expr
  : shift_expr
  | relational_expr LT shift_expr
  | relational_expr GT shift_expr
  | relational_expr LT_EQ shift_expr
  | relational_expr GT_EQ shift_expr
  ;

shift_expr
  : additive_expr
  | shift_expr LT_LT additive_expr
  | shift_expr GT_GT additive_expr
  ;

additive_expr
  : multiplicative_expr
  | additive_expr PLUS multiplicative_expr
  | additive_expr MINUS multiplicative_expr
  ;

multiplicative_expr
  : cast_expr
  | multiplicative_expr STAR cast_expr
  | multiplicative_expr SLASH cast_expr
  | multiplicative_expr PERCENT cast_expr
  ;

cast_expr
  : unary_expr
  | LPAREN type_name RPAREN cast_expr
  ;

unary_expr
  : postfix_expr
  | PLUS_PLUS unary_expr
  | MINUS_MINUS unary_expr
  | unary_operator cast_expr
  | SIZEOF unary_expr
  | SIZEOF LPAREN type_name RPAREN
  ;

unary_operator
  : AMPERSAND
  | STAR
  | PLUS
  | MINUS
  | TILDE
  | BANG
  ;

postfix_expr
  : primary_expr
  | postfix_expr LBRACKET expr RBRACKET
  | postfix_expr LPAREN RPAREN
  | postfix_expr LPAREN argument_expr_list RPAREN
  | postfix_expr DOT ID($id)
  | postfix_expr MINUS_GT ID($id)
  | postfix_expr PLUS_PLUS
  | postfix_expr MINUS_MINUS
  ;

primary_expr
  : ID($id)
  | constant
  | STRING
  | LPAREN expr RPAREN
  ;

argument_expr_list
  : assignment_expr
  | argument_expr_list COMMA assignment_expr
  ;

constant
  : NUM
  ;


/*		           TERMINALS					*/

AMPERSAND
  : '&'($t)
        { OUT($t); }
  ;

AMPERSAND_AMPERSAND
  : '&&'($t)
        { OUT($t); }
  ;

AMPERSAND_EQ
  : '&='($t)
        { OUT($t); }
  ;

AUTO
  : 'auto'($t)
        { OUT($t); }
  ;

BANG
  : '!'($t)
        { OUT($t); }
  ;

BANG_EQ
  : '!='($t)
        { OUT($t); }
  ;

BAR
  : '|'($t)
        { OUT($t); }
  ;

BAR_BAR
  : '||'($t)
        { OUT($t); }
  ;

BAR_EQ
  : '|='($t)
        { OUT($t); }
  ;

BREAK
  : 'break'($t)
        { OUT($t); }
  ;

CARAT
  : '^'($t)
        { OUT($t); }
  ;

CARAT_EQ
  : '^='($t)
        { OUT($t); }
  ;

CASE
  : 'case'($t)
        { OUT($t); }
  ;

CHAR
  : 'char'($t)
        { OUT($t); }
  ;

COLON
  : ':'($t)
        { OUT($t); }
  ;

COMMA
  : ','($t)
        { OUT($t); }
  ;

CONST
  : 'const'($t)
        { OUT($t); }
  ;

CONTINUE
  : 'continue'($t)
        { OUT($t); }
  ;

DEFAULT
  : 'default'($t)
        { OUT($t); }
  ;

DO
  : 'do'($t)
        { OUT($t); }
  ;

DOT
  : '.'($t)
        { OUT($t); }
  ;

DOT_DOT_DOT
  : '...'($t)
        { OUT($t); }
  ;

DOUBLE
  : 'double'($t)
        { OUT($t); }
  ;

ELSE
  : 'else'($t)
        { OUT($t); }
  ;

ENUM
  : 'enum'($t)
        { OUT($t); }
  ;

EQ
  : '='($t)
        { OUT($t); }
  ;

EQ_EQ
  : '=='($t)
        { OUT($t); }
  ;

EXTERN
  : 'extern'($t)
        { OUT($t); }
  ;

FLOAT
  : 'float'($t)
        { OUT($t); }
  ;

FOR
  : 'for'($t)
        { OUT($t); }
  ;

GOTO
  : 'goto'($t)
        { OUT($t); }
  ;

GT
  : '>'($t)
        { OUT($t); }
  ;

GT_EQ
  : '>='($t)
        { OUT($t); }
  ;

GT_GT
  : '>>'($t)
        { OUT($t); }
  ;

GT_GT_EQ
  : '>>='($t)
        { OUT($t); }
  ;

ID(Index $id)
  : ID_TOK($t)
	{ $id= $t.id; OUT($t); 
#if TEST_TYPEDEF
	  printf(" %s", getIDString($id)); 
#endif
        }
  ;

IF
  : 'if'($t)
        { OUT($t); }
  ;

INT
  : 'int'($t)
        { OUT($t); }
  ;

LBRACE
  : '{'($t)
        { OUT($t); }
  ;

LBRACKET
  : '['($t)
        { OUT($t); }
  ;

LONG
  : 'long'($t)
        { OUT($t); }
  ;

LPAREN
  : '('($t)
        { OUT($t); }
  ;

LT
  : '<'($t)
        { OUT($t); }
  ;

LT_EQ
  : '<='($t)
        { OUT($t); }
  ;

LT_LT
  : '<<'($t)
        { OUT($t); }
  ;

LT_LT_EQ
  : '<<='($t)
        { OUT($t); }
  ;

MINUS
  : '-'($t)
        { OUT($t); }
  ;

MINUS_EQ
  : '-='($t)
        { OUT($t); }
  ;

MINUS_GT
  : '->'($t)
        { OUT($t); }
  ;

MINUS_MINUS
  : '--'($t)
        { OUT($t); }
  ;

NUM
  : NUM_TOK($t)
        { OUT($t); }
  ;

PERCENT
  : '%'($t)
        { OUT($t); }
  ;

PERCENT_EQ
  : '%='($t)
        { OUT($t); }
  ;

PLUS
  : '+'($t)
        { OUT($t); }
  ;

PLUS_EQ
  : '+='($t)
        { OUT($t); }
  ;

PLUS_PLUS
  : '++'($t)
        { OUT($t); }
  ;

QUESTION
  : '?'($t)
        { OUT($t); }
  ;

RBRACE
  : '}'($t)
        { OUT($t); }
  ;

RBRACKET
  : ']'($t)
        { OUT($t); }
  ;

REGISTER
  : 'register'($t)
        { OUT($t); }
  ;

RETURN
  : 'return'($t)
        { OUT($t); }
  ;

RPAREN
  : ')'($t)
        { OUT($t); }
  ;

SEMI
  : ';'($t)
        { OUT($t); }
  ;

SHORT
  : 'short'($t)
        { OUT($t); }
  ;

SIGNED
  : 'signed'($t)
        { OUT($t); }
  ;

SIZEOF
  : 'sizeof'($t)
        { OUT($t); }
  ;

SLASH
  : '/'($t)
        { OUT($t); }
  ;

SLASH_EQ
  : '/='($t)
        { OUT($t); }
  ;

STAR
  : '*'($t)
        { OUT($t); }
  ;

STAR_EQ
  : '*='($t)
        { OUT($t); }
  ;

STATIC
  : 'static'($t)
        { OUT($t); }
  ;

STRING			/* catenate adjacent strings */
  : STRING_TOK($t)
        { OUT($t); }
  | STRING STRING_TOK($t)
        { OUT($t); }
  ;

STRUCT
  : 'struct'($t)
        { OUT($t); }
  ;

SWITCH
  : 'switch'($t)
        { OUT($t); }
  ;

TILDE
  : '~'($t)
        { OUT($t); }
  ;

TYPEDEF
  : 'typedef'($t)
        { OUT($t); }
  ;

UNION
  : 'union'($t)
        { OUT($t); }
  ;

UNSIGNED
  : 'unsigned'($t)
        { OUT($t); }
  ;

VOID
  : 'void'($t)
        { OUT($t); }
  ;

VOLATILE
  : 'volatile'($t)
        { OUT($t); }
  ;

WHILE
  : 'while'($t)
        { OUT($t); }
  ;


counter_begin(%in CounterType $counter0)
  : /* empty */
	{ beginCounter($counter0); }
  ;

counter_end(%in CounterType $counter0)
  : /* empty */
	{ endCounter($counter0); }
  ;


%%


/*				IDENTIFIERS				*/

 
static HashTab hashTab; 
typedef enum {
  TYPEDEF_ID= 1,
  NON_TYPEDEF_ID= 2
} IDType;

static VOID
initIDs()
{
  enum {
    INIT_HASH_SIZE= 256,
    MAX_LOAD_FACTOR= 4
  };
  HashOpts opts;
  opts.codeFnP= NULL; opts.equalFnP= NULL;
  opts.initSize= INIT_HASH_SIZE; opts.maxLoadFactor= MAX_LOAD_FACTOR;
  opts.isScoped= TRUE; opts.reorderOK= FALSE;
  hashTab= newHashTab(&opts);
}

static VOID
terminateIDs()
{
  delHashTab(hashTab);
}


static VOID 
dclTypedef(id, isTypedef)
  Index id;
  BooleanX isTypedef;
{
  Index val= isTypedef ? TYPEDEF_ID : NON_TYPEDEF_ID;
  putHashTab(hashTab, (HashKey)id, (HashVal)val);
  VOID_RET();
}

static Boolean 
isTypedef()
{
  CONST Index id= yylval.tok.t.id;
  HashVal v= getHashTab(hashTab, (HashKey)id);
  return v != NULL_HASH_VAL && ((Index)v) == TYPEDEF_ID;
}

static VOID
beginScope()
{
  beginScopeHashTab(hashTab);
}

static VOID
endScope()
{
  endScopeHashTab(hashTab);
}


/*			     MAIN PROGRAM				*/

static Options options;
CONST Options *CONST optionsP= &options;

OptInfo optTab[]= {
  /* LongOpt Name, ShortOpt Name, Argument Type, User ID, CheckP, ValP,
   * OptFn, Doc
   */

  OPT_ENTRY("cond", 'c', NO_OPT_FLAG, COND_OPT,	NULL,
	     (VOIDP) &options.isCond, yesNoOptFn,
            "\
--cond | -c          Profile subexpressions of a ?: conditional expression\n\
                     (default: `0').\n\
"
  ),

  OPT_ENTRY("help", 'h', NO_OPT_FLAG, HELP_OPT,	NULL, NULL,
	     helpOptFn,
            "\
--help | -h          Print summary of options and exit.\n\
"
  ),

  OPT_ENTRY("output", 'o', REQUIRED_OPT_FLAG, OUTPUT_OPT, NULL,
	    (VOIDP) &options.output, stringOptFn,
    "\
--output=OUTFILE |   Use OUTFILE as the name of the generated output\n\
 -o OUTFILE          C file (default: append `_BB').\n\
"
  ),

  OPT_ENTRY("prefix", 'p', REQUIRED_OPT_FLAG, PREFIX_OPT, NULL,
	    (VOIDP) &options.prefix, stringOptFn,
    "\
--prefix=PREFIX |    Use PREFIX as prefix of all generated names in\n\
 -p PREFIX           instrumented file (default: `_BB').\n\
"
  ),

  OPT_ENTRY("preprocess", 'P', OPTIONAL_OPT_FLAG, PREPROCESS_OPT, NULL,
	    (VOIDP) &options.isPreprocess, yesNoOptFn,
    "\
--preprocess | -P    Run preprocessor (given by environmental var CPP) on\n\
                     input file (default: `1').\n\
"
  ),

  OPT_ENTRY("silent", 's', NO_OPT_FLAG, SILENT_OPT, NULL,
	    (VOIDP) &options.isSilent, yesNoOptFn,
    "\
--silent | -s        Do not generate error messages (default: `0').\n\
"
  ),

  OPT_ENTRY(NULL, 'V', SYNONYM_OPT_FLAG, VERSION_OPT, NULL,
	    NULL, NULL, NULL
  ),

  OPT_ENTRY("version", 'v', NO_OPT_FLAG, VERSION_OPT, NULL,
	    NULL, versionOptFn,
    "\
--version | -v | -V  Print version information and exit.\n\
"
  ),

  OPT_ENTRY("typedef", 't', REQUIRED_OPT_FLAG, TYPEDEF_OPT, NULL,
	    NULL, typedefOptFn,
    "\
--typedef=ID |       Declare identifier ID to be a typedef in the global\n\
 -t ID               scope.\n\
"
  )

};


static Int 
helpOptFn(id, checkP, valP, argP)
  Int id; 
  VOIDP checkP;
  VOIDP valP;
  ConstString argP;
{
  allOptsHelp(NULL, "Usage: zprof [options] lex-file.", 
	      optTab, N_ELEMENTS(optTab));
  exit(0);
  return 0;
}

static Int 
typedefOptFn(id, checkP, valP, argP)
  Int id;
  VOIDP checkP;
  VOIDP valP;
  ConstString argP;
{
  dclTypedef(getID(argP, strlen(argP)), TRUE);
  return 0;
}

#ifndef VERSION
#define VERSION	"0.0"
#endif

static Int 
versionOptFn(id, checkP, valP, argP)
  Int id; 
  VOIDP checkP;
  VOIDP valP;
  ConstString argP;
{
  fprintf(stderr, "zprof version %s\n", VERSION);
  exit(0);
  return 0;
}

#ifndef SIGNATURE
#define SIGNATURE "Zerksis"
#endif

/* Insert signature string into executable to permit distinguishing it
 * from other executables of the same name.
 */
char uniqueSignature[]= SIGNATURE;

/* A little function to ensure that above signature doesn't get optimized
 * away by optimizing linkers.
 */
int 
ensureSig()
{
  int n= sizeof(uniqueSignature);
  int i;
  for (i= 0; i < n; i++) uniqueSignature[i]= (char)(2*i);
  return uniqueSignature[2];
}


int 
main PROTO((int argc, ConstString argv[]));

int
main(argc, argv)
  int argc;
  ConstString argv[]; 
{
  int n;
#if YY_ZYACC_DEBUG
  yydebug= 1;
#endif
  ensureSig();
  initOut();
  initScan();
  initIDs();
  options.prefix= "_BB"; options.isPreprocess= 1;
  n= parseOpt(NULL, argc, argv, optTab, N_ELEMENTS(optTab));
  if (n != argc - 1) {
    allOptsHelp(NULL, "Usage: zprof [options] file.", 
	        optTab, N_ELEMENTS(optTab));
    exit(1);
  }
  newFile(argv[n]);
  beginOutFile(argv[n]);
  if (yyparse() != 0) {
    fatal("could not recover from parse errors.");
  }
  putOut(&yylval.tok.t);
  endOutFile();
  terminateIDs();
  terminateScan();
  terminateOut();
  return 0;
}

#if YY_ZYACC_DEBUG

static YY_VOID
tokSemFn(out, tokNum, p) 
  FILE *out;
  int tokNum;
  VOIDP p;
{
  enum { MAX_TOK= 10 };
  YYSTYPE *yylvalP= p;
  CONST int len0= yylvalP->tok.t.len - yylvalP->tok.t.len1;
  CONST int len= (len0 < MAX_TOK) ? len0 : MAX_TOK;
  fprintf(out, "%.*s", len, yylvalP->tok.t.text + yylvalP->tok.t.len1);
}

#endif
