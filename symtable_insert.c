#include "symtable_shared.h"

int symtable_insert(tree_t *tree, char* key, type_t type, bool modifiable, int line){

    if (tree == NULL || key == NULL){
        fprintf(stderr, "%s", "spatne zadane paramery");
        return 1;
    }

    

    tree->root = symtable_insert_recursive(tree->root, key, type, modifiable, line);

    return 0;
}
