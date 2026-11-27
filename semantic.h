/**
 *
 * @file semantic.h
 * @author Daniel Burda (xburdad00), Ema Osmerova (xosmere00)
 * @brief projekt IFJ a IAL
 * @version 0.1
 * @date 2026-11-27
 *
 */
#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "symtable.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief verejna funkce pro kontrolu ast stromu
 *
 * @param tree
 * @return int
 */
int sem_check(ast_tree_t *tree);

#endif