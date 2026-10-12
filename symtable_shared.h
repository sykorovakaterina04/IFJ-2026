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