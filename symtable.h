#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  TYPE_INT,
  TYPE_BOOL,
  TYPE_DOUBLE,
  TYPE_STRING,
  TYPE_UNIT,
  TYPE_ANY
} data_type_t;

typedef enum { FUNCTION, VARIABLE, KONSTANT, PARAM } node_type_t;

typedef struct tdata {
  node_type_t type;
  data_type_t data_type; // aji return type v pripade funkci
  bool modifiable;
  int line;
  int param_count;
  data_type_t *param_types;
  int id; // unikatni cislo promennych u funkci je to vzdy 0, pro shadowing
} tdata_t;

typedef struct tnode {
  char *key;
  tdata_t data;
  struct tnode *left;
  struct tnode *right;
  int height;
} tnode_t;

typedef struct tree {
  tnode_t *root;
  struct tree *parent;
} tree_t;

/**
 * @brief
 *
 * @param parent
 * @return tree_t*
 */
tree_t *symtable_init(tree_t *parent);

/**
 * @brief
 *
 * @param tree
 * @param key
 * @param type
 * @param data
 * @param line
 * @param param_count
 * @param param_types
 * @return int
 */
int symtable_insert(tree_t *tree, const char *key, node_type_t type,
                    data_type_t data, int line);

int symtable_insert_func(tree_t *tree, const char *key, data_type_t data_type,
                         int line, int param_count, data_type_t *param_types);

/**
 * @brief
 *
 * @param tree
 * @param key
 * @return tdata_t*
 */
tdata_t *symtable_lookup(const tree_t *tree, const char *key);

tdata_t *symtable_lookup_local(const tree_t *tree, const char *key);

void symtable_destroy_tree(tree_t *tree);

#endif
