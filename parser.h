/**
 * @file parser.h
 * @brief Rozhraní syntaktického analyzátoru (rekurzivní sestup) pro překladač
 * jazyka IFJ26
 *
 * @author Daniel Doubrava (xdoubrd00)
 */

#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "automat.h"
#include "symtable.h"

/**
 * @brief Sloupec začátku aktuálního logického řádku (kontrola odsazení v
 * závorkách)
 */
extern int parser_line_indent;
/**
 * @brief Zásobník úrovní odsazení bloků
 */
typedef struct {
  int *items;   ///< Pole sloupců jednotlivých úrovní
  int top;      ///< Index vrcholu zásobníku
  int capacity; ///< Alokovaná kapacita
} indent_stack_t;

/**
 * @brief Aktuální token
 */
extern token_t current_token;

/**
 * @brief Jméno funkce s anotací @c \@main
 */
extern char *main_func_name;
/**
 * @brief Načte další token z lexikálního analyzátoru
 * @return Nový token
 */
token_t get_token(void);

/**
 * @brief Nahlédne o jeden token dopředu bez jeho zpracování
 * @return Následující token
 */
token_t peek_token(void);

/**
 * @brief Posune parser na další token (výsledek v ::current_token)
 */
void next(void);

/**
 * @brief Provede syntaktickou analýzu celého programu a vytvoří AST
 * @param[out] out_ast Vytvořený AST (při chybě NULL)
 * @return 0 při úspěchu, jinak chybový kód (1-10, 99)
 */
int parse(ast_tree_t **out_ast);

/**
 * @brief Zkontroluje prolog @c import @c ifj26*
 * @retval 0 Prolog je správný
 * @retval 2 Chybný nebo chybějící prolog
 */
int parse_prologue(void);

/**
 * @brief Zpracuje definici vstupní funkce s anotací @c \@main
 * @param global_symtable Globální tabulka symbolů
 * @param indent_stack Zásobník odsazení
 * @param[out] out_func Uzel definice funkce v AST
 * @return 0 při úspěchu, jinak chybový kód
 */
int parse_main(tree_t *global_symtable, indent_stack_t *indent_stack,
               ast_node_t **out_func);
/**
 * @brief Zpracuje definici uživatelské funkce
 * @param global_symtable Globální tabulka symbolů
 * @param indent_stack Zásobník odsazení
 * @param[out] out_func Uzel definice funkce v AST
 * @return 0 při úspěchu, jinak chybový kód
 */
int parse_function(tree_t *global_symtable, indent_stack_t *indent_stack,
                   ast_node_t **out_func);
/**
 * @brief Zaregistruje vestavěné funkce do globální tabulky symbolů
 * @param global_symtable Globální tabulka symbolů
 * @retval 0 Úspěch
 * @retval 99 Chyba alokace
 */
int register_functions(tree_t *global_symtable);

/**
 * @brief Zpracuje odsazený blok příkazů
 * @param global_symtable Globální tabulka symbolů
 * @param local_symtable Tabulka symbolů bloku
 * @param indent_stack Zásobník odsazení
 * @param func_return_file Návratový typ aktuální funkce
 * @param[out] out_block Uzel bloku v AST
 * @return 0 při úspěchu, jinak chybový kód
 */
int parse_block(tree_t *global_symtable, tree_t *local_symtable,
                indent_stack_t *indent_stack, data_type_t func_return_file,
                ast_node_t **out_block);

#endif