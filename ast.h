#ifndef AST_H
#define AST_H


#include <stdlib.h>
#include "symtable.h"

typedef enum {
    AST_FUNC_DEF, AST_VAR_DEF, AST_ASSIGN, 
    AST_IF, AST_WHILE, AST_CALL, 
    AST_BINOP, AST_VAR, AST_LITERAL, AST_BLOCK,
    AST_RETURN
} ast_type_t;

typedef enum {
    GLOBAL, LOCAL, TMP
} frame_t;

typedef enum {
    ADD, SUB, MUL, DIV, IDIV, EQ, NEQ, LT, GT, LEQ, GEQ
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
            ast_node_t *body;
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
        }block;

        struct {
            ast_node_t *var;
        }node_return;
    }data;
};

typedef struct ast_tree{
    ast_node_t *root;
}ast_tree_t;

ast_tree_t *ast_init();

ast_node_t *ast_create_node(ast_type_t type);

ast_node_t *ast_create_binop(binop_type_t type, ast_node_t *left, ast_node_t *right, int line);

ast_node_t *ast_create_assign(ast_node_t *source, ast_node_t *dest, int line);

ast_node_t *ast_create_block(tree_t *tree, ast_node_t *first_line);

ast_node_t *ast_create_if(ast_node_t *cond, ast_node_t *body, ast_node_t *else_body);

ast_node_t *ast_create_var(char *name, data_type_t type, frame_t frame, int line);

ast_node_t *ast_create_var_def(char *name, data_type_t type, int line);

ast_node_t *ast_create_while(ast_node_t *cond, ast_node_t *body);

ast_node_t *ast_create_func_def(char *name, ast_node_t *args, ast_node_t *body, int line);

ast_node_t *ast_create_func_call(char *name, ast_node_t *args, int line);

ast_node_t *ast_create_literal_int(int value, int line);

ast_node_t *ast_create_literal_double(double value, int line);

ast_node_t *ast_create_literal_string(char *value, int line);

ast_node_t *ast_create_literal_unit(int line);

ast_node_t *ast_create_return(ast_node_t *var, int line);

void ast_destroy_tree(ast_tree_t *tree);

#endif