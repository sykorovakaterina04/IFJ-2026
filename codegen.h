/**
 *
 * @file codegen.h
 * @author Katerina Sykorova (xsykork00)
 * @brief projekt IFJ a IAL 2026
 * @version 0.1
 * @date 2026-11-19
 *
 */

#include "ast.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>

// pomocne funkce

/**
 * @brief pomocna funkce pro tisknuti literalu
 *
 * @param node predany uzel
 */
void print_literal(ast_node_t *node);

/**
 * @brief pomocna funkce pro tisknuti promennych
 *
 * @param node
 */
void print_var(ast_node_t *node);

/**
 * @brief pomocna funkce pro tisknuti definic promennych
 *
 * @param node
 */
void definuj_defvar(ast_node_t *node);

// konec pomocnych funkci

// vbudovane funkce

/**
 * @brief funkce pro vestavenou funkci print(value: Any): Unit
 *
 * @param node
 */
void case_print(ast_node_t *node);

/**
 * @brief funkce pro vestavenou funkci toStr(value: Any): String
 *
 * @param node
 */
void case_toStr(ast_node_t *node);

/**
 * @brief funkce pro vestavenou funkci toInt(value: Double): Int
 *
 * @param node
 */
void case_toInt(ast_node_t *node);

/**
 * @brief funkce pro vestavenou funkci strLength(value: String): Int
 *
 * @param node
 */
void case_strLength(ast_node_t *node);

/**
 * @brief funkce pro vestavenou funkci readLine(): String
 *
 */
void case_readLine(void);

/**
 * @brief funkce pro vestavenou funkci readInt(): Int
 *
 */
void case_readInt(void);

/**
 * @brief funkce pro vestavenou funkci readDouble(): Double
 *
 */
void case_readDouble(void);

/**
 * @brief funkce pro vestavenou funkci ord(value: String, index: Int): Int
 *
 * @param node
 */
void case_ord(ast_node_t *node);

/**
 * @brief funkce pro vestavenou funkci chr(code: Int): String
 *
 * @param node
 */
void case_chr(ast_node_t *node);

/**
 * @brief funkce pro vestavenou funkci subStr(value: String, start: Int, end:
 * Int): String
 *
 * @param node
 */
void case_subStr(ast_node_t *node);

// konec vbudovanych funkci

// funkce zpracovavajici uzly

/**
 * @brief funkce pro generovani literalu
 *
 * @param node
 */
void ast_literal(ast_node_t *node);

/**
 * @brief funkce pro generovani definice promenne
 *
 * @param node
 */
void ast_var_def();

/**
 * @brief funkce pro generovani kodu za pripadu, ze je promenna pouzita (napr.
 * jako parametr)
 *
 * @param node
 */
void ast_var(ast_node_t *node);

/**
 * @brief funkce pro generovani binarnich operaci
 *
 * @param node
 */
void ast_binop(ast_node_t *node);

/**
 * @brief funkce pro pridani hodnoty do promenne
 *
 * @param node
 */
void ast_assign(ast_node_t *node);

/**
 * @brief funkce pro volani funkci
 *
 * @param node
 */
void ast_call(ast_node_t *node);

/**
 * @brief funkce pro genrovani podminenych vyrazu
 *
 * @param node
 */
void ast_if(ast_node_t *node);

/**
 * @brief funkce pro generovani while cyklu
 *
 * @param node
 */
void ast_while(ast_node_t *node);

/**
 * @brief funkce pro zpracovani blokovych useku kodu
 *
 * @param node
 */
void ast_block(ast_node_t *node);

/**
 * @brief funkce pro generovani ukonceni funkce
 *
 * @param node
 */
void ast_return(ast_node_t *node);

/**
 * @brief funkce pro genrovani definice a tela funkce
 *
 * @param node
 */
void ast_func_def(ast_node_t *node);

// konec funkci zpracovavajici uzly

// ridici funkce

/**
 * @brief funkce pro zpracovani uzlu
 *
 * @param node
 */
void zpracuj_uzel(ast_node_t *node);

/**
 * @brief funkce pro pruchod retezce uzlu
 *
 * @param node
 */
void projdi_strom(ast_node_t *node);

/**
 * @brief hlavni funkce geerujici kod
 *
 * @param tree
 */
void generuj_kod(ast_tree_t *tree);

// konec ridicich funkci
