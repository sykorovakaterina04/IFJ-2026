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
} data_type_t; // datove typy

typedef enum {
  FUNCTION,
  VARIABLE,
  KONSTANT,
  PARAM
} node_type_t; // typy, uzlu v tabulce symbolu

typedef struct tdata {
  node_type_t type;      // typ uzlu
  data_type_t data_type; // datovy typ promene, pripadne return type funkce
  bool modifiable;       // jestli je zaznam a promenna obecne modifikovatelna
  int line;              // cislo radku, na kterem se nachazi
  int param_count;       // pocet parametru u funkce
  data_type_t *param_types; // datove typy parametru funkce
  int id;                   // id pro shadowing
} tdata_t;

typedef struct tnode {
  char *key;           // nazev uzlu, podle ktereho se v klada a vyhledava
  tdata_t data;        // data uzlu
  struct tnode *left;  // ukazatel na levy uzel
  struct tnode *right; // ukazatel na pravy uzel
  int height;          // vyska uzlu
} tnode_t;

typedef struct tree {
  tnode_t *root;       // koren binarniho stromu
  struct tree *parent; // ukazatel na tabulku symbolu v nadrazenem ramci
} tree_t;

/**
 * @brief funkce na inicializaci stromu
 *
 * @param parent tabulka symbolu na vyssim scope
 * @return tree_t*
 */
tree_t *symtable_init(tree_t *parent);

/**
 * @brief funkce pro vlozeni zaznamu do tabulky
 *
 * @param tree strom, do ktereho se vklada
 * @param key nazev zaznamu
 * // data zaznamu:
 * @param type
 * @param data
 * @param line
 * @param param_count
 * @param param_types
 * @return int
 */
int symtable_insert(tree_t *tree, const char *key, node_type_t type,
                    data_type_t data, int line);

/**
 * @brief
 *
 * @param tree
 * @param key
 * @param data_type
 * @param line
 * @param param_count
 * @param param_types
 * @return int
 */
int symtable_insert_func(tree_t *tree, const char *key, data_type_t data_type,
                         int line, int param_count, data_type_t *param_types);

/**
 * @brief funkce pro vyhledavani v tabulce a i v tabulkach nadrazenych ramcu
 *
 * @param tree
 * @param key
 * @return tdata_t*
 */
tdata_t *symtable_lookup(const tree_t *tree, const char *key);

/**
 * @brief funkce na vyhledani jen v tabulce daneho ramce
 *
 * @param tree
 * @param key
 * @return tdata_t*
 */
tdata_t *symtable_lookup_local(const tree_t *tree, const char *key);

/**
 * @brief funkce na vymazani stromu
 *
 * @param tree
 */
void symtable_destroy_tree(tree_t *tree);

#endif
