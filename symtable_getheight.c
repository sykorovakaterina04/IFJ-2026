#include "symtable_shared.h"

int symtable_getheight(tnode_t *node){
    return (node == NULL) ? 0 : 
    (node->height);
}