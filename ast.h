#ifndef AST_H
#define AST_H


#include <stdlib.h>
#include "symtable.h"

typedef enum {
    AST_FUNC_DEF, AST_VAR_DEF, AST_ASSIGN, 
    AST_IF, AST_FOR, AST_WHILE, AST_CALL, 
    AST_BINOP, AST_VAR, AST_LITERAL, AST_BLOCK
} ast_type_t;

typedef enum {
    GLOBAL, LOCAL, TMP
} frame_t;

typedef enum {
    ADD, SUB, MUL, DIV, IDIV
} binop_type_t;

typedef enum {
    INT, STRING, DOUBLE
} var_type_t;

typedef struct ast_node ast_node_t;

struct ast_node{
    ast_type_t type;
    data_type_t return_type;

    ast_node_t *next;
    
    int line;

    union {
        struct{
            ast_node_t *dest;
            ast_node_t *source;
        }assign;

        struct{
            ast_node_t *left;
            ast_node_t *right;
            binop_type_t type;
        }binop;

        struct{
            ast_node_t *cond;
            ast_node_t *body;
            ast_node_t *else_body;
        }condicionals;

        struct{
            char *name;
            ast_node_t *args;
        }func;

        struct {
            int i_val;
            double d_val;
            char *string_val;
            data_type_t type;
            data_type_t conversion_type;
        }literal;

        struct {
            char *name;
            data_type_t type;
            data_type_t conversion_type;
            frame_t frame;
        }variable;

        struct { 
            ast_node_t *first_line;
            tree_t *symtable;
        } block;

    }data;
};

typedef struct ast_tree{
    ast_node_t *root;
}ast_tree_t;

ast_tree_t *ast_init();

ast_node_t *ast_create_node(ast_type_t type);

#endif