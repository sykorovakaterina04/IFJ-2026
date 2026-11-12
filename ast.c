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

ast_node_t *ast_create_binop(int op, ast_node_t *left, ast_node_t *right){
    ast_node_t *binop = ast_create_node(AST_BINOP);

    binop->data.binop.op = op;
    binop->data.binop.left = left;
    binop->data.binop.right = right;
}

ast_node_t *ast_create_assign(ast_node_t *left, ast_node_t *right){
    
}
