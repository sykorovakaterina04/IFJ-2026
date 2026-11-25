#ifndef EXPR_PARSER_H
#define EXPR_PARSER_H

#include "ast.h"
#include "automat.h"
#include "symtable.h"
#include <stdbool.h>

int parse_expression_ex(tree_t *local_symtable, data_type_t *out_type,
                        ast_node_t **out_node, bool in_parens);

#endif