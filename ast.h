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
    ast_type_t type; // typ uzlu
    data_type_t return_type; //jaky typ ma node vracet

    ast_node_t *next; //dalsi radek v blocku / dalsi argument / dalsi funkce
    
    int line; // cislo radku, na kterem se operace nachazi

    union {
        struct{
            ast_node_t *dest; //do ceho se bude prirazovat
            ast_node_t *source; //co se priradi
        }assign;

        struct{
            ast_node_t *left; //leva strana operace
            ast_node_t *right; // prava strana operace
            binop_type_t type; // typ operace
        }binop;

        struct{
            ast_node_t *cond; //kondice
            ast_node_t *body; // block kdyz plati
            ast_node_t *else_body; //block kdyz neplati
        }condicionals;

        struct{
            char *name; // nazev funkce
            ast_node_t *args; //argumenty
            ast_node_t *body; //block tela funkce
        }func;

        struct {
            int i_val; //value v pripade integeru
            double d_val; // value v pripade double
            char *string_val; // value v pripade string
            data_type_t type; // datovy typ literalu
            data_type_t conversion_type; // typ, na ktery se ma literal prevest
        }literal;

        struct {
            char *name; // nazev promenne
            data_type_t type; // datovy typ promenne
            data_type_t conversion_type; // typ, na ktery se ma promenna prevest
            frame_t frame; // ramec, ve kterem se promenna nachazi
        }variable;

        struct { 
            ast_node_t *first_line; // prvni radek blocku
            tree_t *symtable; // tabulka symbolu blocku
        }block;

        struct {
            ast_node_t *var; //promenna ci literal, ktery se ma vratit
            ast_node_t *func_def; //ukazatel na func def uzel, ke kteremu patri return
        }node_return;
    }data;
};

typedef struct ast_tree{
    ast_node_t *root;
}ast_tree_t;

ast_tree_t *ast_init();

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

ast_node_t *ast_create_return(ast_node_t *var, ast_node_t *func_def, int line);

void ast_destroy_node(ast_node_t *node);

void ast_destroy_tree(ast_tree_t *tree);

#endif