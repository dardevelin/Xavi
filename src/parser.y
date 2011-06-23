%{
#include "tokenizer.h"
#include "parserx.h"
%}
%left '+' '-'
%left '*' '/'
%nonassoc 'd' UNEG

%token NUMBER
%token UNEG
%token EOL 0
%%
calculation: { vldci_set_value(0); }
 | expression EOL { vldci_set_value($1); }
 ;

expression: NUMBER
 | expression '+' expression { $$ = $1 + $3; }
 | expression '-' expression { $$ = $1 - $3; }
 | expression '*' expression { $$ = $1 * $3; }
 | expression '/' expression { $$ = $1 / $3; }
 | '-' NUMBER %prec UNEG { $$ = $2 * -1; }
 | expression 'd' NUMBER { $$ = vldci_dice($1, $3); }
 | '(' expression ')' { $$ = $2; }
;
%%
