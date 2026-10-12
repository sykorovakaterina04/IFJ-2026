#include "symtable_shared.h"

int symtable_getbalance(tnode_t *node){
    return (node == NULL) ? 0 : 
    symtable_getheight(node->left) - symtable_getheight(node->right);
}