#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef enum {TYPE_INT, TYPE_DOUBLE, TYPE_STRING, TYPE_UNIT} type_t;

struct tree;
typedef struct tree tree_t;

struct tnode;
typedef struct tnode tnode_t;

typedef struct tdata{
    
    type_t type;

    bool modifiable;

    int line;
}tdata_t;

tree_t *symtable_init();

int symtable_insert(tree_t *tree, char *key, type_t type, bool modifiable, int line);

tnode_t *symtable_insert_recursive(tnode_t *node, char *key, type_t type, bool modifiable, int line);

tdata_t *symtable_lookup(const tree_t *tree, char *key);

tnode_t *symtable_rrotate(tnode_t *node);

tnode_t *symtable_lrotate(tnode_t *node);

int symtable_max(int ,int b);

int symtable_getheight(tnode_t *node);

int symtable_getbalance(tnode_t *node);

void symtable_destroy(tnode_t *node);


