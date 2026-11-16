#ifndef EXPR_PARSER_H
#define EXPR_PARSER_H

#include "automat.h"
#include "symtable.h"
#include <stdbool.h>




int parse_expression(tree_t *global_symtable, tree_t *local_symtable, data_type_t *out_type, bool is_condition);


#endif