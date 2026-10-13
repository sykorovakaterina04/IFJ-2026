#include "symtable_shared.h"

void symtable_destroy(tnode_t *node){
    if (node != NULL){
        symtable_destroy(node->left);
        symtable_destroy(node->right);
        free(node->key);
        free(node);
    }

}