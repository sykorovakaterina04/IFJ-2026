#include "ast.h"

ast_tree_t *ast_init(){
    ast_tree_t *ast = calloc(1, sizeof(ast_tree_t));
    if(ast == NULL){
        fprintf(stderr, "%s", "chyba pri alokaci stromu");
        return NULL;
    }
    return ast;
}

ast_node_t *ast_create_node(ast_type_t type){
    ast_node_t *new_node = calloc(1, sizeof(ast_node_t));
    if(new_node == NULL){
        fprintf(stderr, "%s", "chyba pri alokaci stromu");
        exit(99);
    }
    new_node->type = type;
    return new_node;
}

ast_node_t *ast_make_binop(int op, ast_node_t *left, ast_node_t *right){
    ast_node_t *new_node = ast_create_node(AST_BINOP);

    new_node->data.binop.left = left;
    new_node->data.binop.right = right;
    new_node->data.binop.op = op;

    return new_node;
}

ast_node_t *ast_make_var(char* indetifier){
    ast_node_t *new_node = ast_create_node(AST_VAR);
}
