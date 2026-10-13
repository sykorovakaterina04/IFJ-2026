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


