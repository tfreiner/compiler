#ifndef TOKEN_H_
#define TOKEN_H_
#include <stdio.h>

//----Token structure.
//Contains token enum which defines the tokenID, a token instance, a line number, and a token name.

typedef struct token_t{
	enum token{KW_tk, ID_tk, DIGIT_tk, EOF_tk, EQ_tk, GREQGR_tk, LSEQLS_tk, NOT_tk, COL_tk, PLUS_tk, MIN_tk, MUL_tk, DIV_tk, PERIOD_tk, OPENPAR_tk, CLOSEDPAR_tk, COMMA_tk, OPENCURBRACK_tk, CLOSEDCURBRACK_tk, SEMIC_tk, OPENSQBRACK_tk, CLOSEDSQBRACK_tk} tokenID;
	char* tokenInstance;
	int lineNumber;
	const char *tokenName[22];
}token_t;

typedef enum{INITIAL, FINAL, ERROR, NEXT} state_t;

#endif
