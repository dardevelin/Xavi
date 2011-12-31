%{
#include "tokenize.h"
#include "tree.h"
#include "xvcalcix.h"
%}

%code requires {
#include "structs.h"
}
%union {
	tree * t;
	arglist * a;
	char * s;
	int i;
	float f;
}

%left '+' '-'
%left '*' '/'
%nonassoc 'd' UNEG

%token <i> INTEGER
%token <f> FLOAT
%token UNEG
%token <s> ID
%token ERROR
%token EOL 0

%type <t> expression fcall
%type <a> arglist
%type <s> id

%%
calculation: EOL { xvcalc_set_nil(); }
 | expression EOL {
	xvcalc_set_value_from_tree($1);
	xvcalc_clear_memory($1);
};

expression: INTEGER            { $$ = xvcalc_new_int($1);          }
 | '-'      INTEGER %prec UNEG { $$ = xvcalc_new_int($2 * -1);     }
 |          FLOAT              { $$ = xvcalc_new_float($1);        }
 | '-'      FLOAT %prec UNEG   { $$ = xvcalc_new_float($2 * -1.0); }
 | expression '+' expression { $$ = xvcalc_new_operation('+', $1, $3); }
 | expression '-' expression { $$ = xvcalc_new_operation('-', $1, $3); }
 | expression '*' expression { $$ = xvcalc_new_operation('*', $1, $3); }
 | expression '/' expression { $$ = xvcalc_new_operation('/', $1, $3); }
 | expression 'd' INTEGER {
	$$ = xvcalc_new_operation('d', $1, xvcalc_new_int($3));
   }
 | '(' expression ')' { $$ = $2; }
 | fcall
 | ERROR { xvcalc_report_lex_error(*xvcalc_yylval.s); /*xvcalc_error();*/ YYERROR; }
;

fcall: id '(' arglist ')' { $$ = xvcalc_new_function($1, $3); printf("Function(%s)\n", $1);};

id: ID { $$ = xvcalc_make_id(xvcalc_yylval.s); printf("ID(%s)\n", $1); }

arglist: expression { $$ = xvcalc_add_argument($1, NULL); }
 | expression ',' arglist { $$ = xvcalc_add_argument($1, $3); };
;
%%
