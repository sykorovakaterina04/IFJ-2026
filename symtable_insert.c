#include "symtable_shared.h"

int symtable_insert(tree_t *tree, char* key, type_t type){
    tnode_t *new_node = malloc(sizeof(tnode_t));
    if (new_node == NULL){
            fprintf(stderr, "%s", "chyba pri alokace nove polozky");
            return 1;
    }
    new_node->data.key = key;
    new_node->data.type = type;
    new_node->left = NULL;
    new_node->right = NULL;


    if(tree->root == NULL){
        tree->root = new_node;
        return 0;
    }


    tnode_t *tmp = tree->root;
    tnode_t *parent = tmp;
    int cmp;
    while(tmp != NULL){
        cmp = strcmp(key, tmp->data.key);
        parent = tmp;
        if(cmp < 0){
            tmp = tmp->left;
        }
        if(cmp > 0){
            tmp = tmp->right;
        }
        if (cmp == 0){
            fprintf(stderr, "%s", "identifikator uz se v tabulce nachazi");
            free(new_node);
            return 1;
        }
    }
    if(cmp < 0){
        parent->left = new_node;
    }
    if(cmp > 0){
        parent->right = new_node;
    }
    return 0;
}
