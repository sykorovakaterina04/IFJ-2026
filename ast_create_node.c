#include "ast.h"

ast_node_t *ast_create_node(type_t type){
    ast_node_t *new_node = calloc(1, sizeof(ast_node_t));
    if(new_node == NULL){
        fprintf(stderr, "%s", "chyba pri alokaci stromu");
        return NULL;
    }
    new_node->type = type;
    return new_node;
}