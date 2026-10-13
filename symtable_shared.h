#include "symtable.h"

typedef struct tnode{
    char* key;                              
    tdata_t data;
    struct tnode *left;
    struct tnode *right;
    int height;
}tnode_t;

typedef struct tree{
    int height;
    tnode_t *root;
}tree_t;

tnode_t *symtable_rrotate(tnode_t *node);

tnode_t *symtable_lrotate(tnode_t *node);

int symtable_max(int ,int b);

int symtable_getheight(tnode_t *node);

int symtable_getbalance(tnode_t *node);

void symtable_destroy(tnode_t *node);

tnode_t *symtable_insert_recursive(tnode_t *node, char *key, type_t type, bool modifiable, int line);