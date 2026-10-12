#include "symtable_shared.h"

int symtable_getbalance(tnode_t *node){
    if (node == NULL){
        return 0;
    }
    return symtable_getheight(node->left) - symtable_getheight(node->right);
}