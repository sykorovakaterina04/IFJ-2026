#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef enum {TYPE_INT, TYPE_DOUBLE, TYPE_STRING, TYPE_UNIT} type_t;

struct tree;
typedef struct tree tree_t;

typedef struct tdata{
    
    type_t type;

    bool modifiable;

    int line;
}tdata_t;

tree_t *symtable_init();

int symtable_insert(tree_t *tree, char *key, type_t type, bool modifiable, int line);

tdata_t *symtable_lookup(const tree_t *tree, char *key);

void symtable_destroy_tree(tree_t *tree);

tnode_t *symtable_rrotate(tnode_t *node);

tnode_t *symtable_lrotate(tnode_t *node);

int symtable_max(int ,int b);

int symtable_getheight(tnode_t *node);

int symtable_getbalance(tnode_t *node);

void symtable_destroy(tnode_t *node);

tnode_t *symtable_insert_recursive(tnode_t *node, char *key, type_t type, bool modifiable, int line);

tree_t *symtable_init(){
    tree_t * tree = malloc(sizeof(tree_t));
    if(tree == NULL){
        fprintf(stderr, "%s", "chyba pri alokaci stromu");
        return NULL;
    }
    tree->root = NULL;
    return tree;
}

int symtable_insert(tree_t *tree, char* key, type_t type, bool modifiable, int line){

    if (tree == NULL || key == NULL){
        fprintf(stderr, "%s", "spatne zadane paramery");
        return 1;
    }

    

    tree->root = symtable_insert_recursive(tree->root, key, type, modifiable, line);

    return 0;
}

tdata_t * symtable_lookup(const tree_t *tree, char* key){

    tnode_t *tmp = tree->root;

    while(tmp != NULL){
        int cmp = strcmp(key, tmp->key);
        if(cmp < 0){
            tmp = tmp->left;
        }
        if(cmp > 0){
            tmp = tmp->right;
        }
        if (cmp == 0){
            return &(tmp->data);
        }
    }
    fprintf(stderr, "%s", "hodnota se v tabluce nenachazi");
    return NULL;
}

void symtable_destroy_tree(tree_t *tree){
    symtable_destroy(tree->root);
    free(tree);

}

tnode_t *symtable_rrotate(tnode_t *node){
    tnode_t *node_left = node->left;
    tnode_t *node_right = node_left->right;

    node_left->left = node;
    node->right = node_right;

    node->height = symtable_max(symtable_getheight(node->left), symtable_getheight(node->right) + 1);
    node_left->height = symtable_max(symtable_getheight(node_right->left), symtable_getheight(node_left->right) + 1);

    return node;
}

tnode_t * symtable_lrotate(tnode_t *node){
    tnode_t *node_right = node->right;
    tnode_t *node_left = node_right->right;

    node_right->right = node;
    node->left =node_left;

    node->height = symtable_max(symtable_getheight(node->right), symtable_getheight(node->left) + 1);
    node_right->height = symtable_max(symtable_getheight(node_right->left), symtable_getheight(node_right->right) + 1);

    return node;
}

int symtable_max(int a, int b){
    return (a > b) ? a : b;
}

int symtable_getheight(tnode_t *node){
    return (node == NULL) ? 0 : 
    (node->height);
}

int symtable_getbalance(tnode_t *node){
    return (node == NULL) ? 0 : 
    symtable_getheight(node->left) - symtable_getheight(node->right);
}

void symtable_destroy(tnode_t *node){
    if (node != NULL){
        symtable_destroy(node->left);
        symtable_destroy(node->right);
        free(node->key);
        free(node);
    }

}

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

    node->height = symtable_max(symtable_getheight(node->left), symtable_getheight(node->right)) + 1;

    int balance = symtable_getbalance(node);

    if (balance > 1){
        if(strcmp(key, node->left->key) < 0){
            return symtable_rrotate(node);
        }
        if(strcmp(key, node->left->key) < 0){
            node->left = symtable_lrotate(node->left);
            return symtable_rrotate(node);
        }  
    }

    if (balance < -1){
        if(strcmp(key, node->left->key) < 0){
            return symtable_rrotate(node);
        }
        if(strcmp(key, node->left->key) < 0){
            node->right = symtable_rrotate(node->right);
            return symtable_lrotate(node);
        }  
    }

    return node;
}