#include "ast.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>

// pomocne funkce

void print_literal(ast_node_t *node);

void print_var(ast_node_t *node);

void definuj_defvar(ast_node_t *node);

// konec pomocnych funkci

// vbudovane funkce

void case_print(ast_node_t *node);

void case_toStr(ast_node_t *node);

void case_toInt(ast_node_t *node);

void case_strLength(ast_node_t *node);

void case_readLine(void);

void case_readInt(void);

void case_readDouble(void);

void case_ord(ast_node_t *node);

void case_chr(ast_node_t *node);

void case_subStr(ast_node_t *node);

// konec vbudovanych funkci

// funkce zpracovavajici uzly

void ast_literal(ast_node_t *node);

void ast_var_def();

void ast_var(ast_node_t *node);

void ast_binop(ast_node_t *node);

void ast_assign(ast_node_t *node);

void ast_call(ast_node_t *node);

void ast_if(ast_node_t *node);

void ast_while(ast_node_t *node);

void ast_block(ast_node_t *node);

void ast_func_def(ast_node_t *node);

// konec funkci zpracovavajici uzly

// ridici funkce

void zpracuj_uzel(ast_node_t *node);

void projdi_strom(ast_node_t *node);

void generuj_kod(ast_tree_t *tree);

// konec ridicich funkci
