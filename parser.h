#ifndef PARSER_H
#define PARSER_H

#include "automat.h"
#include "symtable.h"

//aktualni token
extern token_t current_token;

token_t get_token();
void next(void);
int parse(void);
int parse_prologue(void);

int parse_function(tree_t *global_symtable);
int parse_main(tree_t *global_symtable);


#endif 