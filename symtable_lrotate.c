#include "symtable_shared.h"

tnode_t * symtable_lrotate(tnode_t *node){
    tnode_t *node_right = node->right;
    tnode_t *node_left = node_right->right;

    node_right->right = node;
    node->left =node_left;

    node->height = symtable_max(symtable_getheight(node->right), symtable_getheight(node->left) + 1);
    node_right->height = symtable_max(symtable_getheight(node_right->left), symtable_getheight(node_right->right) + 1);

    return node;
}