/**
 * @file expr_parser.h
 * @brief Rozhraní precedenčního syntaktického analyzátoru výrazů (PSA) pro
 * jazyk IFJ26.
 *
 * @author Daniel Doubrava (xdoubrd00)
 */

#ifndef EXPR_PARSER_H
#define EXPR_PARSER_H

#include "ast.h"
#include "symtable.h"

/**
 * @brief Precedenční syntaktická analýza výrazu.
 * @param local_symtable Tabulka symbolů pro vyhledání proměnných.
 * @param[out] out_type Typ výrazu.
 * @param[out] out_node Kořen AST výrazu (může být NULL).
 * @param in_parens Výraz je uvnitř závorek (povoluje konce řádků).
 * @retval 0 Úspěch.
 * @retval 2 Syntaktická chyba.
 * @retval 3 Nedefinovaná proměnná.
 * @retval 99 Interní chyba.
 */
int parse_expression_ex(tree_t *local_symtable, data_type_t *out_type,
                        ast_node_t **out_node, bool in_parens);

#endif