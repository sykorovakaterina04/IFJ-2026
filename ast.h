#include <stdlib.h>
#include "symtable.h"

typedef enum {
    AST_FUNC_DEF, AST_VAR_DEF, AST_ASSIGN, 
    AST_IF, AST_WHILE, AST_CALL, 
    AST_BINOP, AST_VAR, AST_LITERAL, AST_BLOCK
} ast_type_t;

typedef enum {
    GLOBAL, LOCAL, TMP
} frame_t;

typedef enum {
    INT, STRING, DOUBLE
} var_type_t;

typedef struct ast_node{
    ast_type_t type;
    type_t return_type;

    struct ast_node *next;
    

    int line;

    union {
        struct{
            struct ast_node *left;
            struct ast_node *right;
            char op;
        }binop;

        struct{
            struct ast_node *cond;
            struct ast_node *body;
            struct ast_node *else_body;
        }condicionals;

        struct{
            char *name;
            struct ast_node *args;
        }func;

        struct {
            int i_val;
            double d_val;
            char *string_val;
        }literal;

        struct {
            char *name;
            frame_t frame;
        }variable;
    }data;
}ast_node_t;

typedef struct ast_tree{
    ast_node_t *root;
}ast_tree_t;

ast_tree_t *ast_init();

ast_node_t *ast_create_node(ast_type_t type);

