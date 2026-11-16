#include "symtable.h"

tree_t *symtable_init(tree_t *parent){
    tree_t * tree = calloc(1, sizeof(tree_t));
    if(tree == NULL){
        fprintf(stderr, "%s", "chyba pri alokaci stromu");
        exit(99);
    }
    tree->parent = parent;
    return tree;
}

static int symtable_getheight(tnode_t *node){
    return (node == NULL) ? 0 : 
    (node->height);
}

static int symtable_max(int a, int b){
    return (a > b) ? a : b;
}

tdata_t * symtable_lookup_local(const tree_t *tree, const char *key){

    if(tree == NULL){
        fprintf(stderr, "%s", "predany strom byl nulovy");
        return NULL;
    }

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
    return NULL;
}

tdata_t * symtable_lookup(const tree_t *tree, const char *key){

    if(tree == NULL){
        fprintf(stderr, "%s", "predany strom byl nulovy");
        return NULL;
    }

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

    if(tree->parent != NULL){
        symtable_lookup(tree->parent, key);
    }
    return NULL;
}

static tnode_t *symtable_rrotate(tnode_t *node){
    tnode_t *new_root = node->left;
    tnode_t *node_left = new_root->right;

    new_root->right = node;
    node->left = node_left;

    node->height = symtable_max(symtable_getheight(node->left), symtable_getheight(node->right)) + 1;
    new_root->height = symtable_max(symtable_getheight(new_root->left), symtable_getheight(new_root->right)) + 1;

    return new_root;
}

static tnode_t * symtable_lrotate(tnode_t *node){
    tnode_t *new_root = node->right;
    tnode_t *node_right = new_root->left;
    new_root->left = node;
    node->right =node_right;

    node->height = symtable_max(symtable_getheight(node->right), symtable_getheight(node->left)) + 1;
    new_root->height = symtable_max(symtable_getheight(new_root->left), symtable_getheight(new_root->right)) + 1;

    return new_root;
}

static int symtable_getbalance(tnode_t *node){
    return (node == NULL) ? 0 : 
    symtable_getheight(node->left) - symtable_getheight(node->right);
}





static tnode_t *symtable_insert_recursive(tnode_t *node,const char *key, node_type_t type, data_type_t data_type, int line, int param_count, data_type_t *param_types){

    if(node == NULL){
        tnode_t *new_node = calloc(1, sizeof(tnode_t));
        if ((new_node == NULL)){
            fprintf(stderr, "%s", "chyba pri alokaci noveho elementu");
            exit(99);
        }
        
        new_node->key = malloc(strlen(key) + 1);
        if(new_node->key == NULL){
            fprintf(stderr, "%s", "chyba pri alokaci noveho elementu");
            exit(99);
        }
        strcpy(new_node->key, key);

        new_node->height = 1;
        new_node->data.type = type;
        new_node->data.data_type = data_type;

        if(type == VARIABLE){
            new_node->data.modifiable = true;
        } else {
            new_node->data.modifiable = false;
        }

        if(new_node->data.type == FUNCTION || new_node->data.type == PARAM){
            new_node->data.active = true;
        }

        new_node->data.line = line;
        new_node->data.param_count = param_count;
        new_node->data.param_types = malloc(param_count * sizeof(data_type_t));

        if(new_node->data.param_types == NULL){
            fprintf(stderr, "%s", "chyba pri alokaci parametru");
            exit(99);
        }

        memcpy(new_node->data.param_types, param_types, param_count * sizeof(data_type_t));
        return(new_node);
    }
    

    if (strcmp(key, node->key) < 0){
        node->left = symtable_insert_recursive(node->left, key, type, data_type, line, param_count, param_types);
    } else if(strcmp(key, node->key) > 0) {
        node->right = symtable_insert_recursive(node->right, key, type, data_type, line, param_count, param_types);
    } else{
        return node;
    }

    node->height = symtable_max(symtable_getheight(node->left), symtable_getheight(node->right)) + 1;

    int balance = symtable_getbalance(node);

    if (balance > 1){
        if(strcmp(key, node->left->key) < 0){
            return symtable_rrotate(node);
        }
        if(strcmp(key, node->left->key) > 0){
            node->left = symtable_lrotate(node->left);
            return symtable_rrotate(node);
        }  
    }

    if (balance < -1){
        if(strcmp(key, node->right->key) > 0){
            return symtable_lrotate(node);
        }
        if(strcmp(key, node->right->key) < 0){
            node->right = symtable_rrotate(node->right);
            return symtable_lrotate(node);
        }  
    }

    return node;
}





int symtable_insert(tree_t *tree, const char *key, node_type_t type, data_type_t data_type, int line){

    if (tree == NULL || key == NULL){
        fprintf(stderr, "%s", "spatne zadane paramery");
        return 1;
    }


    

    tree->root = symtable_insert_recursive(tree->root, key, type, data_type, line, 0, NULL);

    return 0;
}



int symtable_insert_func(tree_t *tree, const char *key, data_type_t data_type, int line, int param_count, data_type_t *param_types){

    if (tree == NULL || key == NULL){
        fprintf(stderr, "%s", "spatne zadane paramery");
        return 1;
    }


    

    tree->root = symtable_insert_recursive(tree->root, key, FUNCTION, data_type, line, param_count, param_types);

    return 0;
}





static void symtable_destroy(tnode_t *node){
    if (node != NULL){
        symtable_destroy(node->left);
        symtable_destroy(node->right);
        free(node->key);

        if(node->data.type == FUNCTION && node->data.param_types != NULL){
            free(node->data.param_types);
        }
        free(node);
    }

}





void symtable_destroy_tree(tree_t *tree){
    if (tree == NULL) return;
    symtable_destroy(tree->root);
    free(tree);

}