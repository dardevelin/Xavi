/* This file is part of Xavi. Copyright 2012 Vincent Damewood. Licensed under
   the GNU Lesser General Public License, version 3. */

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "XaviLexer.h"

#if !defined USE_BISON
#define USE_BISON 0
#endif /* USE_BISON */

#if USE_BISON
#include "XaviYyParser.h"
#else
#error "Building without BISON is currently not supported."
#endif

#define EULER 2.18
#define PI 3.14

enum XaviLexemeId
{
	L_EOI = 0,
	L_INTEGER = 257,	/* [0-9]+ */
	L_FLOAT,			/* [0-9]+\.[0-9]+ */
	L_ID,				/* [a-ce-z][a-z0-9]*)|(d[a-z][a-z0-9]* */
	L_E,				/* "e" */
	L_PI,				/* "pi" */
	L_ERROR
};

enum XaviDfaState
{
	DFA_END = -1,
	DFA_START = 0,
	DFA_E,
	DFA_PI_1,
	DFA_PI_2,
	DFA_DICE,
	DFA_ID,
	DFA_INTEGER,
	DFA_FLOAT,
	DFA_TERM_CHAR,
	DFA_TERM_STRING,
	DFA_TERM_EOI,
	DFA_TERM_ERROR
};

typedef enum XaviLexemeId XaviLexemeId;

static int isOperator(int character)
{
	return (
		character == '+'
		|| character == '-'
		|| character == '/'
		|| character == '*'
		|| character == '^'
		|| character == ','
		|| character == '('
		|| character == ')');
}

static int isIdLead(int character)
{
	return (
		isalpha(character)
		&& character != 'd');
}

int XaviLexerRead(XaviLexer * lexer, YYSTYPE * token)
{
	XaviLexemeId terminal = L_EOI;
	int dfaState = DFA_START;
	size_t length = 0;
	free(lexer->lexeme);
	lexer->lexeme = NULL;

	// FIXME: This code doesn't check for pi or e.
	while (dfaState != -1) {
		switch (dfaState) {
		case DFA_START:
			if(isOperator(*lexer->current)) {
				lexer->current++;
				terminal = *lexer->begin;
				dfaState = DFA_TERM_CHAR;
			}
			else if (*lexer->current == 'd') {
				lexer->current++;
				dfaState = DFA_DICE;
			}
			else if (isdigit(*lexer->current)) {
				lexer->current++;
				dfaState = DFA_INTEGER;
			}
			else if (isIdLead(*lexer->current)) {
				lexer->current++;
				dfaState = DFA_ID;
			}
			else if (isspace(*lexer->current)) {
				lexer->begin++;
				lexer->current++;
			}
			else if (*lexer->current == '\0'){
				dfaState = DFA_TERM_EOI;
			}
			else {
				dfaState = DFA_TERM_ERROR;
			}
			break;
		case DFA_DICE:
			if (isalpha(*lexer->current)) {
				lexer->current++;
				dfaState = DFA_ID;
			}
			else {
				terminal = *lexer->begin;
				dfaState = DFA_TERM_CHAR;
			}
			break;
		case DFA_ID:
			if (isalnum(*lexer->current)) {
				lexer->current++;
			}
			else {
				terminal = L_ID;
				dfaState = DFA_TERM_STRING;
			}
			break;
		case DFA_INTEGER:
			if (*lexer->current == '.') {
				lexer->current++;
				dfaState = DFA_FLOAT;
			}
			else if (isdigit(*lexer->current)) {
				*lexer->current++;
			}
			else {
				terminal = L_INTEGER,
				dfaState = DFA_TERM_STRING;
			}
			break;
		case DFA_FLOAT:
			if (isdigit(*lexer->current)) {
				lexer->current++;
			}
			else {
				terminal = L_FLOAT;
				dfaState = DFA_TERM_STRING;
			}
			break;
		case DFA_TERM_CHAR:
			lexer->lexeme = malloc(2);
			lexer->lexeme[0] = *lexer->begin;
			lexer->lexeme[1] = '\0';
			lexer->begin = lexer->current;
			dfaState = -1;
			break;
		case DFA_TERM_STRING:
			length = lexer->current - lexer->begin;
			lexer->lexeme = malloc(length+1);
			strncpy(lexer->lexeme, lexer->begin, length);
			lexer->lexeme[length] = '\0';			
			lexer->begin = lexer->current;
			dfaState = -1;
			break;
		case DFA_TERM_EOI:
			terminal = EOL;
			dfaState = -1;
			break;
		case DFA_TERM_ERROR:
			terminal = -1;
			break;
		}
	}

	switch (terminal) {
	case L_EOI:
		return EOL;
	case 'd':
	case '+':
	case '-':
	case '*':
	case '/':
	case '^':
	case ',':
	case '(':
	case ')':
		return *lexer->lexeme;
	case L_INTEGER:
		token->i = atoi(lexer->lexeme);
		return INTEGER;
	case L_FLOAT:
		token->f = atof(lexer->lexeme);
		return FLOAT;
	case L_E:
		token->f = EULER;
		return FLOAT;
	case L_PI:
		token->f = PI;
		return FLOAT;
	case L_ID:
		token->s = lexer->lexeme;
		return ID;
	default:
		token->i = 0;
		return ERROR;
	}
}

XaviLexer * XaviLexerNew(const char * inputString)
{
	XaviLexer * rVal = malloc(sizeof(XaviLexer));
	rVal->input = inputString;
	rVal->begin = rVal->input; // Current beginning of read.
	rVal->current = rVal->input; // Current end of read.
	rVal->lexeme = NULL;
	return rVal;
}

void XaviLexerDestroy(XaviLexer** theLexer)
{
	XaviLexer * garbage = *theLexer;
	free(garbage->lexeme);
	free(*theLexer);
	*theLexer = NULL;
}