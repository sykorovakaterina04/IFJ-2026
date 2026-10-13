#include "symtable_shared.h"

void symtable_destroy_tree(tree_t *tree){
    symtable_destroy(tree->root);
    free(tree);

}