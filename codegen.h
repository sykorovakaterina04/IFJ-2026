#include "ast.h"
#include <stdio.h>
#include <string.h>

void print_literal(ast_node_t *node);

void print_var(ast_node_t *node);

void case_print(ast_node_t *node);

void ast_literal(ast_node_t *node);

void ast_var_def(ast_node_t *node);

void ast_var(ast_node_t *node);

void ast_binop(ast_node_t *node);

void ast_assign(ast_node_t *node);

void ast_call(ast_node_t *node);

void projdi_strom(ast_node_t *node);

void generuj_kodu(ast_tree_t *tree);

