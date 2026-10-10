#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

tree_t *symtable_init();

int symtable_insert(tree_t *tree, char* key, type_t type);

tnode_t symtable_lookup(const tree_t *tree, char* key);
