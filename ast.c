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

ast_node_t *ast_create_binop(binop_type_t type, ast_node_t *left, ast_node_t *right, int line){
    ast_node_t *binop = ast_create_node(AST_BINOP);

    binop->data.binop.type = type;
    binop->data.binop.left = left;
    binop->data.binop.right = right;
    binop->line = line;

    return binop;
}

ast_node_t *ast_create_assign(ast_node_t *source, ast_node_t *dest, int line){
    ast_node_t *assign = ast_create_node(AST_ASSIGN);

    assign->data.assign.source = source;
    assign->data.assign.dest = dest;
    assign->line = line;

    return assign;
}

ast_node_t *ast_create_block(tree_t *tree, ast_node_t *first_line){
    ast_node_t *block = ast_create_node(AST_BLOCK);

    block->data.block.first_line = first_line;
    block->data.block.symtable = tree;

    return block;
}

ast_node_t *ast_create_if(ast_node_t *cond, ast_node_t *body, ast_node_t *else_body){
    ast_node_t *conditional = ast_create_node(AST_IF);

    conditional->data.condicionals.cond = cond;
    conditional->data.condicionals.body = body;
    conditional->data.condicionals.else_body = else_body;

    return conditional;
}
