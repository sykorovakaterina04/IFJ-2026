#include "symtable.h"

/**
 * @brief funkce na inicializaci stromu
 * 
 * @param parent tabulka symbolu na vyssim ramci
 * @return tree_t* 
 */
tree_t *symtable_init(tree_t *parent){
    // alokace stromu
    tree_t * tree = calloc(1, sizeof(tree_t));

    //kontrola alokace
    if(tree == NULL){
        exit(99);
    }

    // prirazeni stromu nadrazeneho ramce
    tree->parent = parent;
    return tree;
}

/**
 * @brief vrati vysku uzlu
 * 
 * @param node 
 * @return int 
 */
static int symtable_getheight(tnode_t *node){
    return (node == NULL) ? 0 : 
    (node->height);
}

/**
 * @brief vrati vetsi ze dvou cisel
 * 
 * @param a 
 * @param b 
 * @return int 
 */
static int symtable_max(int a, int b){
    return (a > b) ? a : b;
}

/**
 * @brief funkce na vyhledani jen v tabulce daneho ramce
 * 
 * @param tree 
 * @param key 
 * @return tdata_t* 
 */
tdata_t * symtable_lookup_local(const tree_t *tree, const char *key){

    //kontrola, jestli parametry nejsou NULL
    if(tree == NULL){
        return NULL;
    }

    if(key == NULL ){
        return NULL;
    }

    // zacatek v koreni stromu
    tnode_t *tmp = tree->root;

    
    while(tmp != NULL){

        // pokud je klic mensi nez klic uzlu, na ktery prave ukazuje tmp, tmp jde doleva
        if(strcmp(key, tmp->key) < 0){
            tmp = tmp->left;

        // pokud je vetsi, jde se doprava
        } else if(strcmp(key, tmp->key) > 0){
            tmp = tmp->right;

        // pokud se rovna, jsme na uzlu, ktery hledame
        } else if (strcmp(key, tmp->key) == 0){
            return &(tmp->data);
        }
    }

    // uzel se nenasel
    return NULL;
}

/**
 * @brief funkce pro vyhledavani v tabulce a i v tabulkach nadrazenych ramcu
 * 
 * @param tree 
 * @param key 
 * @return tdata_t* 
 */
tdata_t * symtable_lookup(const tree_t *tree, const char *key){

    // kontrolam jestli parametry nejsou NULL
    if(tree == NULL ){
        return NULL;
    }

    if(key == NULL ){
        return NULL;
    }

    tdata_t *data = NULL;
    
    // dokud existuje tabulka nadrazeneho ramce, prohleda tabulku a presune se na nadrazenou
    while(tree){

        data = symtable_lookup_local(tree, key);
        tree = tree->parent;
        

        if(data) return data;
    }

    // uzel se nenasel
    return NULL;
}
/**
 * @brief funkce, ktera provede pravou rotaci nad uzlu
 * 
 * @param node 
 * @return tnode_t* 
 */
static tnode_t *symtable_rrotate(tnode_t *node){

    // vytyceni uzlu, se kterymi chci v rotaci pracovat
    tnode_t *new_root = node->left;
    tnode_t *node_left = new_root->right;

    // z leveho potomka se stane novy root
    new_root->right = node;

    // byvaly pravy potomek noveho korene se stane levym potomkem byvaleho korene
    node->left = node_left;

    // vypocitani nove vysky uzlu
    node->height = symtable_max(symtable_getheight(node->left), symtable_getheight(node->right)) + 1;
    new_root->height = symtable_max(symtable_getheight(new_root->left), symtable_getheight(new_root->right)) + 1;

    return new_root;
}

static tnode_t * symtable_lrotate(tnode_t *node){

    // vytyceni uzlu, se kterymi chci v rotaci pracovat
    tnode_t *new_root = node->right;
    tnode_t *node_right = new_root->left;

    // z praveho potomka se stane novy root
    new_root->left = node;

    // byvaly levy potomek noveho korene se stane pravym potomkem byvaleho korene
    node->right =node_right;

    // vypocitani nove vysky uzlu
    node->height = symtable_max(symtable_getheight(node->right), symtable_getheight(node->left)) + 1;
    new_root->height = symtable_max(symtable_getheight(new_root->left), symtable_getheight(new_root->right)) + 1;

    return new_root;
}
/**
 * @brief vypocita balanc uzlu
 * 
 * @param node 
 * @return int 
 */
static int symtable_getbalance(tnode_t *node){
    return (node == NULL) ? 0 : 
    symtable_getheight(node->left) - symtable_getheight(node->right);
}





static tnode_t *symtable_insert_recursive(tnode_t *node,const char *key, node_type_t type, data_type_t data_type, int line, int param_count, data_type_t *param_types){

    // pokud je uzel NULL, vytvori tam novy a da do nej vsechny parametry
    if(node == NULL){
        tnode_t *new_node = calloc(1, sizeof(tnode_t));
        if ((new_node == NULL)){
            exit(99);
        }
        
        new_node->key = malloc(strlen(key) + 1);
        if(new_node->key == NULL){
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

        new_node->data.line = line;
        new_node->data.param_count = param_count;
        if(param_types){

            new_node->data.param_types = malloc(param_count * sizeof(data_type_t));

            if(new_node->data.param_types == NULL){
                fprintf(stderr, "%s", "chyba pri alokaci parametru");
                exit(99);
            }

            memcpy(new_node->data.param_types, param_types, param_count * sizeof(data_type_t));

        }

        return(new_node);
    }
    
    // rekurzivni sestup stejny jako u vyhledavani
    if (strcmp(key, node->key) < 0){
        node->left = symtable_insert_recursive(node->left, key, type, data_type, line, param_count, param_types);
    } else if(strcmp(key, node->key) > 0) {
        node->right = symtable_insert_recursive(node->right, key, type, data_type, line, param_count, param_types);
    } else{
        return node;
    }

    // vypocitani vysky daneho uzlu
    node->height = symtable_max(symtable_getheight(node->left), symtable_getheight(node->right)) + 1;

    // pokud je uzel nevyvazeny smerem doleva
    if (symtable_getbalance(node) > 1){

        //pokud je levy potomek nevyvazeny doleva, provede se prava rotace
        if(symtable_getbalance(node->left) >= 0){
            return symtable_rrotate(node);
        }

        // pokud je levy potomek nevyvazeny doprava, provede se nejdrive leva rotace a potom prava rotace
        if(symtable_getbalance(node->left) < 0){
            node->left = symtable_lrotate(node->left);
            return symtable_rrotate(node);
        }  
    }

    // pokud je uzel nevyvazeny smerem doprava
    if (symtable_getbalance(node) < -1){

        //pokud je pravy potomek nevyvazeny doprava, provede se leva rotace
        if(symtable_getbalance(node->right) <= 0){
            return symtable_lrotate(node);
        }

        // pokud je parvy potomek nevyvazeny doleva, provede se nejdrive prava rotace a potom leva rotace
        if(symtable_getbalance(node->right) > 0){
            node->right = symtable_rrotate(node->right);
            return symtable_lrotate(node);
        }  
    }

    return node;
}




/**
 * @brief funkce pro vlozeni parametru, konstanty a promennych
 * 
 * @param tree 
 * @param key 
 * @param type 
 * @param data_type 
 * @param line 
 * @return int 
 */
int symtable_insert(tree_t *tree, const char *key, node_type_t type, data_type_t data_type, int line){

    // pokud jsou ukazatele nulove, nastane chyba
    if (tree == NULL || key == NULL){
        return 1;
    }

    // zacatek rekurzivniho vkladani
    tree->root = symtable_insert_recursive(tree->root, key, type, data_type, line, 0, NULL);

    return 0;
}


/**
 * @brief funkce pro vkladani funkci
 * 
 * @param tree 
 * @param key 
 * @param data_type 
 * @param line 
 * @param param_count 
 * @param param_types 
 * @return int 
 */
int symtable_insert_func(tree_t *tree, const char *key, data_type_t data_type, int line, int param_count, data_type_t *param_types){

    // pokud jsou ukazatele nulove, nastane chyba
    if (tree == NULL || key == NULL){
        return 1;
    }

    // zacatek rekurzivniho vkladani
    tree->root = symtable_insert_recursive(tree->root, key, FUNCTION, data_type, line, param_count, param_types);

    return 0;
}




/**
 * @brief funkce pro uvolneni jednotlivych uzlu
 * 
 * @param node 
 */
static void symtable_destroy(tnode_t *node){
    if (node != NULL){

        // uvolneni leveho a praveho potomka
        symtable_destroy(node->left);
        symtable_destroy(node->right);

        // uvolneni klice
        free(node->key);

        // pokud je uzel typu funkce, uvolni i parametry
        if(node->data.type == FUNCTION){
            free(node->data.param_types);
        }

        // uvolneni samotneho uzlu
        free(node);
    }

}




/**
 * @brief funkce na vymazani stromu
 * 
 * @param tree 
 */
void symtable_destroy_tree(tree_t *tree){
    if (tree == NULL) return;

    // rekurzivni uvolneni vsech uzlu
    symtable_destroy(tree->root);

    // uvolneni samotneho stromu
    free(tree);

}