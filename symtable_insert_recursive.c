#include "symtable_shared.h"

tnode_t *symtable_insert_recursive(tnode_t *node, char* key, type_t type, bool modifiable, int line){

    if(node == NULL){
        tnode_t *new_node = malloc(sizeof(tnode_t));
        new_node->key = key;
        new_node->height = 1;
        new_node->data.type = type;
        new_node->data.modifiable = modifiable;
        new_node->data.line = line;
    }
    

    if (strcmp(key, node->key) < 0){
        node->left = symtable_insert_recursive(node->left, key, type, modifiable, line);
    } else if(strcmp(key, node->key) > 0) {
        node->right = symtable_insert_recursive(node->right, key, type, modifiable, line);
    } else{
        return node;
    }

    node->height = symtable_max(symtable_getheight(node->left), symtable_getheight(node->left)) + 1;

    int balance = symtable_getbalance(node);

    if (balance > 1 && strcmp(key, node->left->key) < 0){
        return symtable_rrotate(node);
    }
    if (balance < -1 && strcmp(key, node->right->key) > 0){
        return symtable_lrotate(node);
    }
    if (balance > 1 && strcmp(key, node->left->key) < 0){
        node->left = symtable_lrotate(node->left);
        return symtable_rrotate(node);
    }
    if (balance < -1 && strcmp(key, node->right->key) > 0){
        node->right = symtable_rrotate(node->right);
        return symtable_lrotate(node);
    }

    return node;
}