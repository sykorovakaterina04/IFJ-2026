#include "automat.h"

struct token {
	token_type type;
	union {
		long cislo;
		double desetine_cislo;
		// tady asi potreba vyresit dynamickou alokaci
		char text[50];
	};
};

typedef enum { KA_START } ka_states;

token_t automat(FILE *fptr) {

	token_t token;
	int state = KA_START;
	int c;
	// nebo tady asi potreba vyresit dynamickou alokaci
	char tmp[50];
	int tmp_length = 0;

	while ((c = getchar()) != EOF ||
		   (c != isspace())) { // zatim ignoruji odsazovani

		tmp[tmp_length++] = c;
		tmp[tmp_length] = '\0';

		switch (state) {
		case KA_START:
			if (c == '+') {
				token.type = SCITANI;
				strcpy(token.text, tmp);
				// return token;
			}
			if (c == '*') {
				token.type = NASOBENI;
				strcpy(token.text, tmp);
			}
			if (c == '/') {
				token.type = DELENI;
				strcpy(token.text, tmp);
			}
		}
	}

	return token;
}