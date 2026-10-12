#include "symtable_shared.h"

tnode_t *symtable_rrotate(tnode_t *node){
    tnode_t *node_left = node->left;
    tnode_t *node_right = node_left->right;

    node_left->left = node;
    node->right = node_right;

    node->height = symtable_max(symtable_getheight(node->left), symtable_getheight(node->right) + 1);
    node_left->height = symtable_max(symtable_getheight(node_right->left), symtable_getheight(node_left->right) + 1);

    return node;
}