#include "symtable_shared.h"

int symtable_getheight(tnode_t *node){
    if (node == NULL){
        return 0;
    }
    return node->height;
}