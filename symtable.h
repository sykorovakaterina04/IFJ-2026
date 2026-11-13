#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef enum {TYPE_INT, TYPE_DOUBLE, TYPE_STRING, TYPE_UNIT, TYPE_ANY} data_type_t;

typedef enum {FUNCTION, VARIABLE, KONSTANT, PARAM} node_type_t;

typedef struct tdata{
    node_type_t type;
    data_type_t data_type; //aji return type v pripade funkci
    bool modifiable;
    int line;
    int param_count;
    data_type_t *param_types;
}tdata_t;

typedef struct tnode{
    const char* key;
    tdata_t data;
    struct tnode *left;
    struct tnode *right;
    int height;
}tnode_t;

typedef struct tree{
    int height;
    tnode_t *root;
    struct tree *parent;
}tree_t;

tree_t *symtable_init(tree_t *parent);

int symtable_insert(tree_t *tree, const char *key, node_type_t type, int line, int param_count, data_type_t *param_types);

tdata_t *symtable_lookup(const tree_t *tree, const char *key);

void symtable_destroy_tree(tree_t *tree);


