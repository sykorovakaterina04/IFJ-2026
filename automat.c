#include "automat.h"
#include <ctype.h>

#define KEYWORDS_COUNT (sizeof(keywords) / sizeof(keywords[0]))

typedef enum {
	KA_START,
	KA_GREATER,
	KA_LESS,
	KA_ASSIGN,
	KA_EXCLAMATION,
	KA_ID,
	KA_AT_M,
	KA_AT_A,
	KA_AT_I,
	KA_AT_N,
	KA_AT_MAIN_END,
	KA_SLASH,
	KA_LINE_COMMENT,
	KA_BLOCK_COMMENT,
	KA_BLOCK_COMMENT_END,
	KA_BLOCK_COMMENT_SLASH,
	KA_AFTER_BLOCK_COMMENT_NL,
	KA_STRING_FIRST_QUOTE,
	KA_STRING_SECOND_QUOTE,
	KA_STRING_BODY,
	KA_MULTILINE_STRING,
	KA_STRING_ESCAPE,
	KA_MULTILINE_STRING_END_1,
	KA_MULTILINE_STRING_END_2
} ka_states;

int str_init(string_t *s) {
	s->capacity = 16;
	s->data = (char *)malloc(s->capacity * sizeof(char));
	if (s->data == NULL) {
		fprintf(stderr, "Chyba: neuspesny malloc!\n");
		return 0;
	}
	s->length = 0;
	s->data[0] = '\0';
	return 1;
}

int str_add_char(string_t *s, char c) {
	if (s->length + 1 >= s->capacity) {
		char *tmp;
		s->capacity = 2 * s->capacity;
		tmp = realloc(s->data, s->capacity);
		if (tmp == NULL) {
			fprintf(stderr, "Chyba: neuspesny realloc!\n");
			return 0;
		}
		s->data = tmp;
	}
	s->data[s->length] = c;
	s->length++;
	s->data[s->length] = '\0';
	return 1;
}

void str_clear(string_t *s) {
	s->length = 0;
	if (s->data != NULL) {
		s->data[0] = '\0';
	}
}

void str_free(string_t *s) {
	if (s->data != NULL) {
		free(s->data);
		s->data = NULL;
	}
	s->length = 0;
	s->capacity = 0;
}

typedef struct {
	const char *keyword;
	token_type_t type;
} keyword_entry_t;

static const keyword_entry_t keywords[] = {
	{"def", KEYWORD_DEF},		{"do", KEYWORD_DO},
	{"Double", KEYWORD_DOUBLE}, {"else", KEYWORD_ELSE},
	{"if", KEYWORD_IF},			{"import", KEYWORD_IMPORT},
	{"Int", KEYWORD_INT},		{"return", KEYWORD_RETURN},
	{"String", KEYWORD_STRING}, {"then", KEYWORD_THEN},
	{"Unit", KEYWORD_UNIT},		{"val", KEYWORD_VAL},
	{"var", KEYWORD_VAR},		{"while", KEYWORD_WHILE},
};

token_type_t keyword_type(const char *str) {
	for (size_t i = 0; i < KEYWORDS_COUNT; i++) {
		if (strcmp(str, keywords[i].keyword) == 0) {
			return keywords[i].type;
		}
	}
	return TOKEN_IDENTIFIER;
}

token_t automat(FILE *fptr) {

	string_t buffer;
	token_t token;
	token.str_val = NULL;
	int state = KA_START;
	int c;
	int comment_nesting = 0;
	int comment_has_nl = 0;

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
				state = KA_SLASH;
				break;
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
				state = KA_AT_M;
				break;
			}
			if (isalpha(c) || c == '_') {
				str_init(&buffer);
				str_add_char(&buffer, c);
				state = KA_ID;
				break;
			}
			if (c == '"') {
				str_init(&buffer);
				state = KA_STRING_FIRST_QUOTE;
				break;
			}
			fprintf(stderr, "Lexikalni chyba: neocekavany znak '%c'\n", c);
			exit(51);
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
				exit(51);
			}
			break;
		case KA_ID:
			if (isalnum(c) || c == '_') {
				str_add_char(&buffer, c);
			} else {
				ungetc(c, fptr);
				token.type = keyword_type(buffer.data);
				if (token.type == TOKEN_IDENTIFIER) {
					token.str_val =
						(char *)malloc((buffer.length + 1) * sizeof(char));
					if (token.str_val == NULL) {
						str_free(&buffer);
						fprintf(stderr, "Chyba: neuspesny malloc!\n");
						exit(60);
					}
					strcpy(token.str_val, buffer.data);
				} else {
					token.str_val = NULL;
				}
				str_free(&buffer);
				return token;
			}
			break;
		case KA_AT_M:
			if (c == 'm') {
				state = KA_AT_A;
			} else {
				exit(51);
			}
			break;
		case KA_AT_A:
			if (c == 'a') {
				state = KA_AT_I;
			} else {
				exit(51);
			}
			break;
		case KA_AT_I:
			if (c == 'i') {
				state = KA_AT_N;
			} else {
				exit(51);
			}
			break;
		case KA_AT_N:
			if (c == 'n') {
				state = KA_AT_MAIN_END;
			} else {
				exit(51);
			}
			break;
		case KA_AT_MAIN_END:
			if (!(isalnum(c)) && c != '_') {
				ungetc(c, fptr);
				token.type = TOKEN_AT_MAIN;
				return token;
			} else {
				exit(51);
			}
			break;
		case KA_SLASH:
			if (c == '/') {
				state = KA_LINE_COMMENT;
				break;
			} else if (c == '*') {
				comment_nesting = 1;
				comment_has_nl = 0;
				state = KA_BLOCK_COMMENT;
				break;
			} else {
				ungetc(c, fptr);

				token.type = TOKEN_DIV;
				token.str_val = NULL;
				return token;
			}
		case KA_LINE_COMMENT:
			if (c == '\n') {
				state = KA_START;
			} else if (c == EOF) {
				ungetc(c, fptr);
				state = KA_START;
			}
			break;
		case KA_BLOCK_COMMENT:
			if (c == '*') {
				state = KA_BLOCK_COMMENT_END;
			} else if (c == '/') {
				state = KA_BLOCK_COMMENT_SLASH;
			} else if (c == EOF) {
				fprintf(stderr,
						"Lexikalni chyba: Nedokonceny blokovy komentar\n");
				exit(51);
			} else if (c == '\n') {
				comment_has_nl++;
			}
			break;
		case KA_BLOCK_COMMENT_END:
			if (c == '/') {
				comment_nesting--;
				if (comment_nesting == 0) {
					if (comment_has_nl > 0) {
						state = KA_AFTER_BLOCK_COMMENT_NL;
					} else {
						state = KA_START;
					}
				} else {
					state = KA_BLOCK_COMMENT;
				}
			} else if (c == '*') {
				state = KA_BLOCK_COMMENT_END;
			} else if (c == EOF) {
				fprintf(stderr,
						"Lexikalni chyba: Nedokonceny blokovy komentar\n");
				exit(51);
			} else if (c == '\n') {
				comment_has_nl++;
			} else {
				state = KA_BLOCK_COMMENT;
			}
			break;
		case KA_BLOCK_COMMENT_SLASH:
			if (c == '*') {
				comment_nesting++;
				state = KA_BLOCK_COMMENT;
			} else if (c == '/') {
				state = KA_BLOCK_COMMENT_SLASH;
			} else if (c == EOF) {
				fprintf(stderr,
						"Lexikalni chyba: Nedokonceny blokovy komentar\n");
				exit(51);
			} else if (c == '\n') {
				comment_has_nl++;
			} else {
				state = KA_BLOCK_COMMENT;
			}
			break;
		case KA_AFTER_BLOCK_COMMENT_NL:
			if (c == ' ' || c == '\t' || c == '\r') {
				break;
			} else if (c == '\n') {
				state = KA_START;
				break;
			} else if (c == EOF) {
				ungetc(c, fptr);
				state = KA_START;
				break;
			} else {
				fprintf(stderr,
						"Lexikalni chyba: za vice radkovym blokovym komentarem "
						"nesmi byt token na stejnem radku\n");
				exit(51);
			}
			break;
		case KA_STRING_FIRST_QUOTE:
			if (c == '"') {
				state = KA_STRING_SECOND_QUOTE;
			} else if (c == '\\') {
				state = KA_STRING_ESCAPE;
			} else if (c == '\n' || c == EOF) {
				fprintf(stderr,
						"Lexikalni chyba: Neukonceny retezcovy literal\n");
				exit(51);
			} else {
				str_add_char(&buffer, c);
				state = KA_STRING_BODY;
			}
			break;
		case KA_STRING_SECOND_QUOTE:
			if (c == '"') {
				state = KA_MULTILINE_STRING;
			} else {
				ungetc(c, fptr);
				token.type = TOKEN_STRING;
				token.str_val = buffer.data;
				return token;
			}
			break;
		case KA_STRING_BODY:
			if (c == '"') {
				token.type = TOKEN_STRING;
				token.str_val = buffer.data;
				return token;
			} else if (c == '\\') {
				state = KA_STRING_ESCAPE;
			} else if (c == '\n' || c == EOF) {
				fprintf(stderr,
						"Lexikalni chyba: Neukonceny retezcovy literal\n");
				exit(51);
			} else {
				str_add_char(&buffer, c);
			}
			break;
		case KA_MULTILINE_STRING:
			if (c == '"') {
				state = KA_MULTILINE_STRING_END_1;
			} else if (c == EOF) {
				fprintf(stderr,
						"Lexikalni chyba: Neukonceny retezcovy literal\n");
				exit(51);
			} else {
				str_add_char(&buffer, c);
			}
			break;
		case KA_STRING_ESCAPE:
			if (c == 'n') {
				str_add_char(&buffer, '\n');
				state = KA_STRING_BODY;
			} else if (c == 'r') {
				str_add_char(&buffer, '\r');
				state = KA_STRING_BODY;
			} else if (c == 't') {
				str_add_char(&buffer, '\t');
				state = KA_STRING_BODY;
			} else if (c == '\\') {
				str_add_char(&buffer, '\\');
				state = KA_STRING_BODY;
			} else if (c == '"') {
				str_add_char(&buffer, '\"');
				state = KA_STRING_BODY;
			} else {
				fprintf(stderr, "Lexikalni chyba: Neplatna escape sekvence\n");
				exit(51);
			}
			break;
		case KA_MULTILINE_STRING_END_1:
			if (c == '"') {
				state = KA_MULTILINE_STRING_END_2;
			} else if (c == EOF) {
				fprintf(stderr,
						"Lexikalni chyba: Neukonceny retezcovy literal\n");
				exit(51);
			} else {
				str_add_char(&buffer, '"');
				str_add_char(&buffer, c);
				state = KA_MULTILINE_STRING;
			}
			break;
		case KA_MULTILINE_STRING_END_2:
			if (c == '"') {
				token.type = TOKEN_STRING;
				token.str_val = buffer.data;
				return token;
			} else if (c == EOF) {
				fprintf(stderr,
						"Lexikalni chyba: Neukonceny retezcovy literal\n");
				exit(51);
			} else {
				str_add_char(&buffer, '"');
				str_add_char(&buffer, '"');
				str_add_char(&buffer, c);
				state = KA_MULTILINE_STRING;
			}
			break;
		}
	}

	token.type = TOKEN_EOF;
	token.str_val = NULL;
	return token;
}