#include "symtable_shared.h"

tree_t *symtable_init(){
    tree_t * tree = malloc(sizeof(tree_t));
    tree->root = NULL;
    tree->size = 0;
    return tree;
}
