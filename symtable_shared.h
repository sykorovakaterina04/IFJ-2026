#include "symtable.h"




typedef struct tnode{
    tdata_t data;
    struct tnode *left;
    struct tnode *right;
}tnode_t;

typedef struct tree{
    tnode_t *root;
    size_t size;
}tree_t;