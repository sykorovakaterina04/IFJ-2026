#include "ast.h"

ast_tree_t *ast_init(){
    ast_tree_t *ast = calloc(1, sizeof(ast_tree_t));
    if(ast == NULL){
        fprintf(stderr, "%s", "chyba pri alokaci stromu");
        return NULL;
    }
    return ast;
}
