#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
	// ridici tokeny
	TOKEN_EOF = 0,
	TOKEN_EOL,

	// identifikatory a literaly
	TOKEN_IDENTIFIER,
	TOKEN_INT,
	TOKEN_DOUBLE,
	TOKEN_STRING,

	// operatory
	TOKEN_MUL,
	TOKEN_DIV,
	TOKEN_ADD,
	TOKEN_SUB,
	TOKEN_GREATER,
	TOKEN_GREATER_EQUAL,
	TOKEN_LESS,
	TOKEN_LESS_EQUAL,
	TOKEN_EQ,
	TOKEN_NEQ,

	// oddelovace a interpunkce
	TOKEN_ASSIGN,
	TOKEN_LPAREN,
	TOKEN_RPAREN,
	TOKEN_COLON,
	TOKEN_COMMA,
	TOKEN_SEMICOLON,
	TOKEN_DOT,
	TOKEN_AT_MAIN,

	// klicova slova
	KEYWORD_DEF,
	KEYWORD_DO,
	KEYWORD_DOUBLE,
	KEYWORD_ELSE,
	KEYWORD_IF,
	KEYWORD_IMPORT,
	KEYWORD_INT,
	KEYWORD_RETURN,
	KEYWORD_STRING,
	KEYWORD_THEN,
	KEYWORD_UNIT,
	KEYWORD_VAL,
	KEYWORD_VAR,
	KEYWORD_WHILE,

	// klicova slova pro rozsireni
	KEYWORD_BOOLEAN,
	KEYWORD_CASE,
	KEYWORD_CLASS,
	KEYWORD_EXTENSION,
	KEYWORD_FOR,
	KEYWORD_LIST,
	KEYWORD_MATCH,
	KEYWORD_TYPE,

} token_type_t;

typedef struct {
	char *data;
	int length;
	int capacity;
} string_t;

typedef struct {
	token_type_t type;
	int line;
	int col;
	union {
		long long int_val;
		double double_val;
		char *str_val;
	};
} token_t;

token_t automat(FILE *fptr);

int str_init(string_t *s);

int str_add_char(string_t *s, char c);

void str_free(string_t *s);