#include "symtable_shared.h"

tdata_t * symtable_lookup(const tree_t *tree, char* key){

    tnode_t *tmp = tree->root;

    while(tmp != NULL){
        int cmp = strcmp(key, tmp->key);
        if(cmp < 0){
            tmp = tmp->left;
        }
        if(cmp > 0){
            tmp = tmp->right;
        }
        if (cmp == 0){
            return &(tmp->data);
        }
    }
    fprintf(stderr, "%s", "hodnota se v tabluce nenachazi");
    return NULL;
}