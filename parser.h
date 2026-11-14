#ifndef PARSER_H
#define PARSER_H

#include "automat.h"
#include "symtable.h"
#include "stdbool.h"

// zasobnik urovni odsazeni
typedef struct {
    int *items;
    int top;
    int capacity;
} indent_stack_t;

//aktualni token
extern token_t current_token;

// zakladni parser
token_t get_token();
void next(void);
int parse(void);
int parse_prologue(void);

//funkce
int parse_block(tree_t *global_symtable, tree_t *local_symtable, indent_stack_t *indent_stack)
int parse_main(tree_t *global_symtable, indent_stack_t *indent_stack);
int parse_function(tree_t *global_symtable, indent_stack_t *indent_stack);
int register_functions(tree_t *global_symtable);

//bloky 
int parse_block(tree_t *global_symtable,tree_t *local_symtable, indent_stack_t *indent_stack)

#endif