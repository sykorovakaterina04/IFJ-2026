#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef enum {TYPE_INT, TYPE_DOUBLE, TYPE_STRING, TYPE_UNIT} type_t;

typedef struct tdata{
    
    type_t type;

    bool modifiable;

    int line;
}tdata_t;

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

tree_t *symtable_init();

int symtable_insert(tree_t *tree, char *key, type_t type, bool modifiable, int line);

tdata_t *symtable_lookup(const tree_t *tree, char *key);

void symtable_destroy_tree(tree_t *tree);

static tnode_t *symtable_rrotate(tnode_t *node);

static tnode_t *symtable_lrotate(tnode_t *node);

static int symtable_max(int ,int b);

static int symtable_getheight(tnode_t *node);

static int symtable_getbalance(tnode_t *node);

static void symtable_destroy(tnode_t *node);

static tnode_t *symtable_insert_recursive(tnode_t *node, char *key, type_t type, bool modifiable, int line);

