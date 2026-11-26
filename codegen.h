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

//pomocne funkce

/**
 * @brief pomocna funkce pro tisknuti literalu
 * 
 * @param node predany uzel typu AST_LITERAL
 */
void print_literal(ast_node_t *node);

/**
 * @brief pomocna funkce pro tisknuti promennych
 * 
 * @param node uzel typu AST_VAR nebo AST_VAR_DEF
 */
void print_var(ast_node_t *node);

/**
 * @brief pomocna funkce pro tisknuti definic promennych
 * 
 * @param node Koren podstromu
 */
void definuj_defvar(ast_node_t *node);

//konec pomocnych funkci

//vbudovane funkce

/**
 * @brief funkce pro vestavenou funkci print(value: Any): Unit
 * 
 * @param node uzel pro predani parametru funkce
 */
void case_print(ast_node_t *node);

/**
 * @brief funkce pro vestavenou funkci toStr(value: Any): String 
 * 
 * @param node uzel pro predani parametru funkce
 */
void case_toStr(ast_node_t *node);

/**
 * @brief funkce pro vestavenou funkci toInt(value: Double): Int
 * 
 * @param node uzel pro predani parametru funkce
 */
void case_toInt(ast_node_t *node);

/**
 * @brief funkce pro vestavenou funkci strLength(value: String): Int
 * 
 * @param node uzel pro predani parametru funkce
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
 * @param node uzel pro predani parametru funkce
 */
void case_ord(ast_node_t *node);

/**
 * @brief funkce pro vestavenou funkci chr(code: Int): String
 * 
 * @param node uzel pro predani parametru funkce
 */
void case_chr(ast_node_t *node);

/**
 * @brief funkce pro vestavenou funkci subStr(value: String, start: Int, end: Int): String
 * 
 * @param node uzel pro predani parametru funkce
 */
void case_subStr(ast_node_t *node);

//konec vbudovanych funkci

//funkce zpracovavajici uzly

/**
 * @brief funkce pro generovani literalu
 * 
 * @param node zpracovavany uzel typu AST_LITERAL
 */
void ast_literal(ast_node_t *node);

/**
 * @brief funkce pro generovani definice promenne
 * 
 */
void ast_var_def();

/**
 * @brief funkce pro generovani kodu za pripadu, ze je promenna pouzita (napr. jako parametr)
 * 
 * @param node zpracovavany uzel typu AST_VAR
 */
void ast_var(ast_node_t *node);

/**
 * @brief funkce pro generovani binarnich operaci
 * 
 * @param node zpracovavany uzel typu AST_BINOP
 */
void ast_binop(ast_node_t *node);

/**
 * @brief funkce pro pridani hodnoty do promenne
 * 
 * @param node zpracovavany uzel typu AST_ASSIGN
 */
void ast_assign(ast_node_t *node);

/**
 * @brief funkce pro volani funkci
 * 
 * @param node zpracovavany uzel typu AST_CALL
 */
void ast_call(ast_node_t *node);

/**
 * @brief funkce pro genrovani podminenych vyrazu
 * 
 * @param node zpracovavany uzel typu AST_IF
 */
void ast_if(ast_node_t *node);

/**
 * @brief funkce pro generovani while cyklu
 * 
 * @param node zpracovavany uzel typu AST_WHILE
 */
void ast_while(ast_node_t *node);

/**
 * @brief funkce pro zpracovani blokovych useku kodu
 * 
 * @param node zpracovavany uzel typu AST_BLOCK
 */
void ast_block(ast_node_t *node);

/**
 * @brief funkce pro generovani ukonceni funkce
 * 
 * @param node zpracovavany uzel typu AST_RETURN
 */
void ast_return(ast_node_t *node);

/**
 * @brief funkce pro genrovani definice a tela funkce
 * 
 * @param node zpracovavany uzel typu AST_FUNC_DEF
 */
void ast_func_def(ast_node_t *node);

//konec funkci zpracovavajici uzly

//ridici funkce

/**
 * @brief funkce pro zpracovani uzlu
 * 
 * @param node zpracovavany uzel
 */
void zpracuj_uzel(ast_node_t *node);

/**
 * @brief funkce pro pruchod retezce uzlu
 * 
 * @param node pocatecni uzel zpracovavaneho retezce
 */
void projdi_strom(ast_node_t *node);

/**
 * @brief hlavni funkce geerujici kod
 * 
 * @param tree predany strom pro generovani kodu
 */
void generuj_kod(ast_tree_t *tree);

//konec ridicich funkci

