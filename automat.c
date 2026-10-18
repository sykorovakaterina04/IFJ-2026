#include "automat.h"
#include <ctype.h>

typedef enum {
	KA_START,
	KA_GREATER,
	KA_LESS,
	KA_ASSIGN,
	KA_EXCLAMATION
} ka_states;

token_t automat(FILE *fptr) {

	token_t token;
	token.str_val = NULL;
	int state = KA_START;
	int c;

	while (1) {
		c = fgetc(fptr);
		switch (state) {
		case KA_START:
			if (c == EOF) {
				token.type = TOKEN_EOF;
				token.str_val = NULL;
				return token;
			}
			if (isspace(c)) {
				continue; // preskoceni bilych znaku
			}
			if (c == '+') {
				token.type = TOKEN_ADD;
				token.str_val = NULL;
				return token;
			}
			if (c == '*') {
				token.type = TOKEN_MUL;
				token.str_val = NULL;
				return token;
			}
			if (c == '/') {
				token.type = TOKEN_DIV;
				token.str_val = NULL;
				return token;
			}
			if (c == '-') {
				token.type = TOKEN_SUB;
				token.str_val = NULL;
				return token;
			}
			if (c == '>') {
				state = KA_GREATER;
				break;
			}
			if (c == '<') {
				state = KA_LESS;
				break;
			}
			if (c == '=') {
				state = KA_ASSIGN;
				break;
			}
			if (c == '!') {
				state = KA_EXCLAMATION;
				break;
			}
			if (c == '(') {
				token.type = TOKEN_LPAREN;
				token.str_val = NULL;
				return token;
			}
			if (c == ')') {
				token.type = TOKEN_RPAREN;
				token.str_val = NULL;
				return token;
			}
			if (c == ':') {
				token.type = TOKEN_COLON;
				token.str_val = NULL;
				return token;
			}
			if (c == ',') {
				token.type = TOKEN_COMMA;
				token.str_val = NULL;
				return token;
			}
			if (c == ';') {
				token.type = TOKEN_SEMICOLON;
				token.str_val = NULL;
				return token;
			}
			if (c == '.') {
				token.type = TOKEN_DOT;
				token.str_val = NULL;
				return token;
			}
			if (c == '@') {
				token.type = TOKEN_AT;
				token.str_val = NULL;
				return token;
			}
			break;
		case KA_LESS:
			if (c == '=') {
				token.type = TOKEN_LESS_EQUAL;
				token.str_val = NULL;
				return token;
			} else {
				ungetc(c, fptr);

				token.type = TOKEN_LESS;
				token.str_val = NULL;
				return token;
			}
			break;
		case KA_GREATER:
			if (c == '=') {
				token.type = TOKEN_GREATER_EQUAL;
				token.str_val = NULL;
				return token;
			} else {
				ungetc(c, fptr);

				token.type = TOKEN_GREATER;
				token.str_val = NULL;
				return token;
			}
			break;
		case KA_ASSIGN:
			if (c == '=') {
				token.type = TOKEN_EQ;
				token.str_val = NULL;
				return token;
			} else {
				ungetc(c, fptr);

				token.type = TOKEN_ASSIGN;
				token.str_val = NULL;
				return token;
			}
			break;
		case KA_EXCLAMATION:
			if (c == '=') {
				token.type = TOKEN_NEQ;
				token.str_val = NULL;
				return token;
			} else {
				fprintf(stderr, "Lexikalni chyba: neplatny znak za '!'\n");
				exit(1);
			}
			break;
		}
	}

	token.type = TOKEN_EOF;
	token.str_val = NULL;
	return token;
}