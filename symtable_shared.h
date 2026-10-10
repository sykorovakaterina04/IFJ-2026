#include "symtable.h"


typedef enum {TYPE_INT, TYPE_DOUBLE, TYPE_STRING, TYPE_UNIT} type_t;

typedef struct tdata{
    char* key;
    type_t type;
}tdata_t;

typedef struct tnode{
    tdata_t data;
    struct tnode *left;
    struct tnode *right;
}tnode_t;

typedef struct tree{
    tnode_t *root;
    size_t size;
}tree_t;