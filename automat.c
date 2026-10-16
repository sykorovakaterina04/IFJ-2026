#include "automat.h"

typedef enum { KA_START } ka_states;

token_t automat(FILE *fptr) {

	token_t token;
	int state = KA_START;
	int c;
	char tmp[50];
	int tmp_length = 0;

	while ((c = getchar()) != EOF ||
		   (c != isspace())) { // zatim ignoruji odsazovani

		tmp[tmp_length++] = c;
		tmp[tmp_length] = '\0';

		switch (state) {
		case KA_START:
			if (c == '+') {
				token.type = TOKEN_ADD;
				strcpy(token.str_val, tmp);
				// return token;
			}
			if (c == '*') {
				token.type = TOKEN_MUL;
				strcpy(token.str_val, tmp);
			}
			if (c == '/') {
				token.type = TOKEN_DIV;
				strcpy(token.str_val, tmp);
			}
		}
	}

	return token;
}