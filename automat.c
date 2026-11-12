#include "automat.h"
#include <ctype.h>

#define INITIAL_STRING_CAPACITY 16
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
	KA_MULTILINE_STRING_END_2,
	KA_INT,
	KA_ZERO,
	KA_INT_UNDERSCORE,
	KA_DOUBLE_DOT,
	KA_DOUBLE_DECIMAL,
	KA_DOUBLE_UNDERSCORE,
	KA_DOUBLE_SUFFIX,
	KA_DOUBLE_EXP,
	KA_DOUBLE_EXP_SIGN,
	KA_DOUBLE_EXP_DIGITS,
	KA_DOUBLE_EXP_UNDERSCORE
} ka_states;

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

static int current_line = 1;
static int current_col = 0;
static int prev_col = 0;
static int tab_seen = 0;
static int at_line_start = 1;

int str_init(string_t *s) {
	s->capacity = INITIAL_STRING_CAPACITY;
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

void str_free(string_t *s) {
	if (s->data != NULL) {
		free(s->data);
		s->data = NULL;
	}
	s->length = 0;
	s->capacity = 0;
}

token_type_t keyword_type(const char *str) {
	for (size_t i = 0; i < KEYWORDS_COUNT; i++) {
		if (strcmp(str, keywords[i].keyword) == 0) {
			return keywords[i].type;
		}
	}
	return TOKEN_IDENTIFIER;
}

int my_fgetc(FILE *fptr) {
	int c = fgetc(fptr);
	if (c == '\n') {
		current_line++;
		prev_col = current_col;
		current_col = 0;
	} else if (c == EOF) {
		return c;
	} else {
		current_col++;
	}
	return c;
}

void my_ungetc(int c, FILE *fptr) {
	ungetc(c, fptr);
	if (c == '\n') {
		current_line--;
		current_col = prev_col;
	} else {
		current_col--;
	}
}

token_t automat(FILE *fptr) {

	string_t buffer;
	token_t token;
	token.str_val = NULL;
	int state = KA_START;
	int c;
	int comment_nesting = 0;
	int comment_has_nl = 0;
	int token_start_line = 0;
	int token_start_col = 0;

	while (1) {
		c = my_fgetc(fptr);
		switch (state) {
		case KA_START:
			if (c == EOF) {
				token.type = TOKEN_EOF;
				token.str_val = NULL;
				token.line = current_line;
				token.col = current_col;
				return token;
			}
			if (c == '\n') {
				at_line_start = 1;
				tab_seen = 0;

				token.type = TOKEN_EOL;
				token.str_val = NULL;
				token.line = current_line - 1;
				token.col = prev_col;
				return token;
			}
			if (isspace(c)) {
				if (at_line_start == 1) {
					if (c == '\t') {
						tab_seen = 1;
					}
				}
				continue; // preskoceni bilych znaku
			}
			if (at_line_start == 1 && tab_seen == 1) {
				fprintf(stderr,
						"Lexikalni chyba: Tabulator na neprazdnem radku\n");
				exit(1);
			}
			at_line_start = 0;
			token_start_line = current_line;
			token_start_col = current_col;
			if (c == '+') {
				token.type = TOKEN_ADD;
				token.str_val = NULL;
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			}
			if (c == '*') {
				token.type = TOKEN_MUL;
				token.str_val = NULL;
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			}
			if (c == '/') {
				state = KA_SLASH;
				break;
			}
			if (c == '-') {
				token.type = TOKEN_SUB;
				token.str_val = NULL;
				token.line = token_start_line;
				token.col = token_start_col;
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
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			}
			if (c == ')') {
				token.type = TOKEN_RPAREN;
				token.str_val = NULL;
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			}
			if (c == ':') {
				token.type = TOKEN_COLON;
				token.str_val = NULL;
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			}
			if (c == ',') {
				token.type = TOKEN_COMMA;
				token.str_val = NULL;
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			}
			if (c == ';') {
				token.type = TOKEN_SEMICOLON;
				token.str_val = NULL;
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			}
			if (c == '.') {
				token.type = TOKEN_DOT;
				token.str_val = NULL;
				token.line = token_start_line;
				token.col = token_start_col;
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
			if (c == '0') {
				str_init(&buffer);
				str_add_char(&buffer, c);
				state = KA_ZERO;
				break;
			}
			if (c >= '1' && c <= '9') {
				str_init(&buffer);
				str_add_char(&buffer, c);
				state = KA_INT;
				break;
			}
			fprintf(stderr, "Lexikalni chyba: neocekavany znak '%c'\n", c);
			exit(1);
		case KA_LESS:
			if (c == '=') {
				token.type = TOKEN_LESS_EQUAL;
				token.str_val = NULL;
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			} else {
				my_ungetc(c, fptr);

				token.type = TOKEN_LESS;
				token.str_val = NULL;
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			}
			break;
		case KA_GREATER:
			if (c == '=') {
				token.type = TOKEN_GREATER_EQUAL;
				token.str_val = NULL;
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			} else {
				my_ungetc(c, fptr);

				token.type = TOKEN_GREATER;
				token.str_val = NULL;
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			}
			break;
		case KA_ASSIGN:
			if (c == '=') {
				token.type = TOKEN_EQ;
				token.str_val = NULL;
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			} else {
				my_ungetc(c, fptr);

				token.type = TOKEN_ASSIGN;
				token.str_val = NULL;
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			}
			break;
		case KA_EXCLAMATION:
			if (c == '=') {
				token.type = TOKEN_NEQ;
				token.str_val = NULL;
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			} else {
				fprintf(stderr, "Lexikalni chyba: neplatny znak za '!'\n");
				exit(1);
			}
			break;
		case KA_ID:
			if (isalnum(c) || c == '_') {
				str_add_char(&buffer, c);
			} else {
				my_ungetc(c, fptr);
				token.type = keyword_type(buffer.data);
				if (token.type == TOKEN_IDENTIFIER) {
					token.str_val =
						(char *)malloc((buffer.length + 1) * sizeof(char));
					if (token.str_val == NULL) {
						str_free(&buffer);
						fprintf(stderr, "Chyba: neuspesny malloc!\n");
						exit(99);
					}
					strcpy(token.str_val, buffer.data);
				} else {
					token.str_val = NULL;
				}
				str_free(&buffer);
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			}
			break;
		case KA_AT_M:
			if (c == 'm') {
				state = KA_AT_A;
			} else {
				exit(1);
			}
			break;
		case KA_AT_A:
			if (c == 'a') {
				state = KA_AT_I;
			} else {
				exit(1);
			}
			break;
		case KA_AT_I:
			if (c == 'i') {
				state = KA_AT_N;
			} else {
				exit(1);
			}
			break;
		case KA_AT_N:
			if (c == 'n') {
				state = KA_AT_MAIN_END;
			} else {
				exit(1);
			}
			break;
		case KA_AT_MAIN_END:
			if (!(isalnum(c)) && c != '_') {
				my_ungetc(c, fptr);
				token.type = TOKEN_AT_MAIN;
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			} else {
				exit(1);
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
				my_ungetc(c, fptr);

				token.type = TOKEN_DIV;
				token.str_val = NULL;
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			}
		case KA_LINE_COMMENT:
			if (c == '\n') {
				state = KA_START;
			} else if (c == EOF) {
				my_ungetc(c, fptr);
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
				exit(1);
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
				exit(1);
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
				exit(1);
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
				my_ungetc(c, fptr);
				state = KA_START;
				break;
			} else {
				fprintf(stderr,
						"Lexikalni chyba: za vice radkovym blokovym komentarem "
						"nesmi byt token na stejnem radku\n");
				exit(1);
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
				exit(1);
			} else {
				str_add_char(&buffer, c);
				state = KA_STRING_BODY;
			}
			break;
		case KA_STRING_SECOND_QUOTE:
			if (c == '"') {
				state = KA_MULTILINE_STRING;
			} else {
				my_ungetc(c, fptr);
				token.type = TOKEN_STRING;

				token.str_val =
					(char *)malloc((buffer.length + 1) * sizeof(char));
				if (token.str_val == NULL) {
					str_free(&buffer);
					fprintf(stderr, "Chyba: neuspesny malloc!\n");
					exit(99);
				}
				strcpy(token.str_val, buffer.data);

				str_free(&buffer);
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			}
			break;
		case KA_STRING_BODY:
			if (c == '"') {
				token.type = TOKEN_STRING;
				token.str_val =
					(char *)malloc((buffer.length + 1) * sizeof(char));
				if (token.str_val == NULL) {
					str_free(&buffer);
					fprintf(stderr, "Chyba: neuspesny malloc!\n");
					exit(99);
				}
				strcpy(token.str_val, buffer.data);

				str_free(&buffer);
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			} else if (c == '\\') {
				state = KA_STRING_ESCAPE;
			} else if (c == '\n' || c == EOF) {
				fprintf(stderr,
						"Lexikalni chyba: Neukonceny retezcovy literal\n");
				exit(1);
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
				exit(1);
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
				exit(1);
			}
			break;
		case KA_MULTILINE_STRING_END_1:
			if (c == '"') {
				state = KA_MULTILINE_STRING_END_2;
			} else if (c == EOF) {
				fprintf(stderr,
						"Lexikalni chyba: Neukonceny retezcovy literal\n");
				exit(1);
			} else {
				str_add_char(&buffer, '"');
				str_add_char(&buffer, c);
				state = KA_MULTILINE_STRING;
			}
			break;
		case KA_MULTILINE_STRING_END_2:
			if (c == '"') {
				token.type = TOKEN_STRING;

				token.str_val =
					(char *)malloc((buffer.length + 1) * sizeof(char));
				if (token.str_val == NULL) {
					str_free(&buffer);
					fprintf(stderr, "Chyba: neuspesny malloc!\n");
					exit(99);
				}
				strcpy(token.str_val, buffer.data);

				str_free(&buffer);
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			} else if (c == EOF) {
				fprintf(stderr,
						"Lexikalni chyba: Neukonceny retezcovy literal\n");
				exit(1);
			} else {
				str_add_char(&buffer, '"');
				str_add_char(&buffer, '"');
				str_add_char(&buffer, c);
				state = KA_MULTILINE_STRING;
			}
			break;
		case KA_ZERO:
			if (isdigit(c)) {
				fprintf(stderr, "Lexikalni chyba: Cislo nesmi zacinat nulou\n");
				exit(1);
			} else if (c == '_') {
				fprintf(stderr, "Lexikalni chyba: Za nulou nesmi byt '_'\n");
				exit(1);
			} else if (c == '.') {
				str_add_char(&buffer, c);
				state = KA_DOUBLE_DOT;
			} else if (c == 'e' || c == 'E') {
				str_add_char(&buffer, c);
				state = KA_DOUBLE_EXP;
			} else {
				my_ungetc(c, fptr);
				token.type = TOKEN_INT;
				token.int_val = 0;
				str_free(&buffer);
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			}
			break;
		case KA_INT:
			if (isdigit(c)) {
				str_add_char(&buffer, c);
				state = KA_INT;
			} else if (c == '_') {
				state = KA_INT_UNDERSCORE;
			} else if (c == '.') {
				str_add_char(&buffer, c);
				state = KA_DOUBLE_DOT;
			} else if (c == 'e' || c == 'E') {
				str_add_char(&buffer, c);
				state = KA_DOUBLE_EXP;
			} else {
				my_ungetc(c, fptr);
				token.type = TOKEN_INT;
				token.int_val = strtoll(buffer.data, NULL, 10);
				str_free(&buffer);
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			}
			break;
		case KA_INT_UNDERSCORE:
			if (isdigit(c)) {
				str_add_char(&buffer, c);
				state = KA_INT;
			} else {
				fprintf(stderr, "Lexikalni chyba: Neplatny format cisla int\n");
				exit(1);
			}
			break;
		case KA_DOUBLE_DOT:
			if (isdigit(c)) {
				str_add_char(&buffer, c);
				state = KA_DOUBLE_DECIMAL;
			} else {
				fprintf(stderr,
						"Lexikalni chyba: Neplatny format cisla double\n");
				exit(1);
			}
			break;
		case KA_DOUBLE_DECIMAL:
			if (isdigit(c)) {
				str_add_char(&buffer, c);
				state = KA_DOUBLE_DECIMAL;
			} else if (c == '_') {
				state = KA_DOUBLE_UNDERSCORE;
			} else if (c == 'd' || c == 'D') {
				state = KA_DOUBLE_SUFFIX;
			} else if (c == 'e' || c == 'E') {
				str_add_char(&buffer, c);
				state = KA_DOUBLE_EXP;
			} else {
				my_ungetc(c, fptr);
				token.double_val = strtod(buffer.data, NULL);
				token.type = TOKEN_DOUBLE;
				str_free(&buffer);
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			}
			break;
		case KA_DOUBLE_UNDERSCORE:
			if (isdigit(c)) {
				str_add_char(&buffer, c);
				state = KA_DOUBLE_DECIMAL;
			} else {
				fprintf(stderr,
						"Lexikalni chyba: Neplatny format cisla double\n");
				exit(1);
			}
			break;
		case KA_DOUBLE_SUFFIX:
			if (isalnum(c) || c == '_') {
				fprintf(stderr,
						"Lexikalni chyba: Neplatny znak za sufixem 'd'/'D'\n");
				exit(1);
			} else {
				my_ungetc(c, fptr);
				token.double_val = strtod(buffer.data, NULL);
				token.type = TOKEN_DOUBLE;
				str_free(&buffer);
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			}
			break;
		case KA_DOUBLE_EXP:
			if (c == '+' || c == '-') {
				str_add_char(&buffer, c);
				state = KA_DOUBLE_EXP_SIGN;
			} else if (isdigit(c)) {
				str_add_char(&buffer, c);
				state = KA_DOUBLE_EXP_DIGITS;
			} else {
				fprintf(stderr, "Lexikalni chyba: Neplatny format exponentu\n");
				exit(1);
			}
			break;

		case KA_DOUBLE_EXP_SIGN:
			if (isdigit(c)) {
				str_add_char(&buffer, c);
				state = KA_DOUBLE_EXP_DIGITS;
			} else {
				fprintf(stderr, "Lexikalni chyba: Neplatny format exponentu\n");
				exit(1);
			}
			break;
		case KA_DOUBLE_EXP_DIGITS:
			if (isdigit(c)) {
				str_add_char(&buffer, c);
				state = KA_DOUBLE_EXP_DIGITS;
			} else if (c == '_') {
				state = KA_DOUBLE_EXP_UNDERSCORE;
			} else if (c == 'd' || c == 'D') {
				state = KA_DOUBLE_SUFFIX;
			} else {
				my_ungetc(c, fptr);
				token.double_val = strtod(buffer.data, NULL);
				token.type = TOKEN_DOUBLE;
				str_free(&buffer);
				token.line = token_start_line;
				token.col = token_start_col;
				return token;
			}
			break;
		case KA_DOUBLE_EXP_UNDERSCORE:
			if (isdigit(c)) {
				str_add_char(&buffer, c);
				state = KA_DOUBLE_EXP_DIGITS;
			} else {
				fprintf(stderr, "Lexikalni chyba: Neplatny format exponentu\n");
				exit(1);
			}
			break;
		}
	}

	token.type = TOKEN_EOF;
	token.str_val = NULL;
	token.line = token_start_line;
	token.col = token_start_col;
	return token;
}