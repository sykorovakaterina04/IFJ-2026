#include "automat.h"
#include <ctype.h>

typedef enum { KA_START } ka_states;

token_t automat(FILE *fptr) {

	token_t token;
	token.str_val = NULL;
	int state = KA_START;
	int c;

	while ((c = fgetc(fptr)) != EOF) {
		switch (state) {
		case KA_START:
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
			break;
		}
	}

	token.type = TOKEN_EOF;
	return token;
}