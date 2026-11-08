#include <stdlib.h>
#include "symtable.h"

typedef enum {
    AST_FUNC_DEF, AST_VAR_DEF, AST_ASSIGN, 
    AST_IF, AST_WHILE, AST_CALL, 
    AST_BINOP, AST_VAR, AST_LITERAL, AST_BLOCK
} ast_type_t;

typedef struct ast_node{
    ast_type_t type;
    type_t return_type;

    int line;

    union {
        struct{
            ast_node_t *left;
            ast_node_t *right;
            char znak;
        }binop;

        struct{
            ast_node_t *cond;
            ast_node_t *body;
            ast_node_t *else_body;
        }condicionals;

        struct{
            char *name;
            struct ast_node_t *args;
        }func;

        struct {
            int i_val;
            double d_val;
            char *string;
        }literal;
    };
}ast_node_t;

typedef struct ast_tree{
    ast_node_t *root;
}ast_tree_t;