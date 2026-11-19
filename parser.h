#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "automat.h"
#include "symtable.h"
#include <stdbool.h>

// zasobnik urovni odsazeni
typedef struct {
    int *items;
    int top;
    int capacity;
} indent_stack_t;

//aktualni token
extern token_t current_token;
extern tree_t *global_symtable;
extern char *main_func_name;
// zakladni parser
token_t get_token(void);
token_t peek_token(void);
void next(void);
int parse(ast_tree_t **out_ast);
int parse_prologue(void);

//funkce
int parse_main(tree_t *global_symtable, indent_stack_t *indent_stack,ast_node_t **out_func);
int parse_function(tree_t *global_symtable, indent_stack_t *indent_stack,ast_node_t **out_func);
int register_functions(tree_t *global_symtable);

//bloky 
int parse_block(tree_t *global_symtable,tree_t *local_symtable, indent_stack_t *indent_stack,data_type_t func_return_file, ast_node_t **out_block);

#endif