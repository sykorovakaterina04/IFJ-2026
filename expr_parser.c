/**
 * @file expr_parser.c
 * @brief Implementace precedenční syntaktické analýzy výrazů (PSA) a tvorby AST podstromů
 *
 *  @author Daniel Doubrava (xdoubrd00)
 */

#include "expr_parser.h"
#include "parser.h"
#include <stdlib.h>
#include <string.h>

static data_type_t binop_result_type(binop_type_t op, data_type_t l,
                                     data_type_t r);
// symboly precedenci synt analyzy

/**
 * @brief Symboly precedenční analýzy
 *
 * Terminály a pomocné symboly na zásobníku
 */
enum {
  P_UN = 0, // unarni + -  // -a
  P_MUL,    // * /
  P_ADD,    // binarni + - // a - b
  P_REL,    // < <= > >=
  P_EQ,     // == !=
  P_LP,     // (
  P_RP,     // )
  P_ID,     // operand (i)
  P_END,    // DNO
  P_TERM_COUNT, // Počet terminálů (velikost tabulky)
  SYM_E = P_TERM_COUNT, // neterminal E
  SYM_HANDLE            // zarazka <
};

/**
 * @brief Precedenční tabulka
 *
 * Řádek odpovídá nejvyššímu terminálu na zásobníku, sloupec vstupnímu symbolu
 * Hodnoty: @c '<' odložit (vložit zarážku), @c '=' uzavřít závorku,
 * @c '>' redukovat, @c 'F' úspěšný konec výrazu, @c ' ' chyba
 */
static const char PSA_TABLE[P_TERM_COUNT][P_TERM_COUNT] = {
    //UN  MUL  ADD  REL  EQ    (    )    I    DNO
    {'<', '>', '>', '>', '>', '<', '>', '<', '>'}, // UN
    {'<', '>', '>', '>', '>', '<', '>', '<', '>'}, // MUL
    {'<', '<', '>', '>', '>', '<', '>', '<', '>'}, // ADD
    {'<', '<', '<', ' ', '>', '<', '>', '<', '>'}, // REL
    {'<', '<', '<', '<', ' ', '<', '>', '<', '>'}, // EQ
    {'<', '<', '<', '<', '<', '<', '=', '<', ' '}, // (
    {' ', '>', '>', '>', '>', ' ', '>', ' ', '>'}, // )
    {' ', '>', '>', '>', '>', ' ', '>', ' ', '>'}, // I
    {'<', '<', '<', '<', '<', '<', ' ', '<', 'F'}, // DNO
};

/**
 * @brief Položka zásobníku precedenční analýzy
 */
typedef struct {
  int sym;          ///< Symbol 
  token_type_t tok; ///< Původní token 
  ast_node_t *node; ///< Uzel AST 
  data_type_t type; ///< Datový typ výrazu
  int line;         ///< Řádek ve zdrojovém kódu
} psa_item_t;

/**
 * @brief Zásobník precedenční syntaktické analýzy
 */
typedef struct {
  psa_item_t *items; ///< Pole položek
  int top; ///< Počet prvků na zásobníku
  int cap; ///< Alokovaná kapacita
} psa_stack_t;

/**
 * @brief Převede token operátoru na typ binární operace v AST
 * @param tok Typ tokenu
 * @return Odpovídající typ
 */
static binop_type_t tok_to_binop(token_type_t tok) {
  switch (tok) {
  case TOKEN_ADD:
    return ADD;
  case TOKEN_SUB:
    return SUB;
  case TOKEN_MUL:
    return MUL;
  case TOKEN_DIV:
    return DIV;
  case TOKEN_LESS:
    return LT;
  case TOKEN_LESS_EQUAL:
    return LEQ;
  case TOKEN_GREATER:
    return GT;
  case TOKEN_GREATER_EQUAL:
    return GEQ;
  case TOKEN_EQ:
    return EQ;
  default:
    return NEQ;
  }
}

/**
 * @brief Vloží položku na zásobník (podle potřeby zvětší kapacitu)
 * @param stack Zásobník
 * @param it Vkládaná položka
 * @retval 0 Úspěch
 * @retval 99 Chyba alokace
 */
static int st_push(psa_stack_t *stack, psa_item_t it) {
  if (stack->top >= stack->cap) {
    int nc = stack->cap ? stack->cap * 2 : 16;
    psa_item_t *tmp = realloc(stack->items, nc * sizeof(psa_item_t));
    if (tmp == NULL) {
      return 99;
    }
    stack->items = tmp;
    stack->cap = nc;
  }
  stack->items[stack->top++] = it;
  return 0;
}

/**
 * @brief Najde nejvyšší terminál na zásobníku
 * @param stack Zásobník
 * @return Index nejvyššího terminálu
 */
static int st_top_terminal(const psa_stack_t *stack) {
  for (int i = stack->top - 1; i >= 0; i--) {
    if (stack->items[i].sym < P_TERM_COUNT) {
      return i;
    }
  }
  return 0;
}

/**
 * @brief Vloží zarážku @c < za položku na pozici @p idx
 * @param stack Zásobník
 * @param idx Index položky, za kterou se zarážka vloží
 * @retval 0 Úspěch
 * @retval 99 Chyba alokace
 */
static int st_insert_handle_after(psa_stack_t *stack, int idx) {
  psa_item_t h = {0};
  h.sym = SYM_HANDLE;
  if (st_push(stack, h) != 0)
    return 99;
  memmove(&stack->items[idx + 2], &stack->items[idx + 1],
          (size_t)(stack->top - 1 - (idx + 1)) * sizeof(psa_item_t));
  stack->items[idx + 1] = h;
  return 0;
}

/**
 * @brief Provede redukci podle pravidel gramatiky výrazů
 *
 * Pravidla: E -> i, E -> +E | -E, E -> E op E, E -> (E). Při redukci se
 * sestavuje AST a zjišťuje typ výrazu. Unární mínus se převádí na @c 0 - E
 * @param stack Zásobník
 * @retval 0 Úspěch
 * @retval 2 Žádné pravidlo neodpovídá
 * @retval 99 Chyba alokace
 */
static int psa_reduce(psa_stack_t *stack) {
  int h = stack->top - 1;
  while (h >= 0 && stack->items[h].sym != SYM_HANDLE) {
    h--;
  }
  if (h < 0) {
    return 2;
  }
  int k = stack->top - h - 1; // délka pravé strany
  psa_item_t *r = &stack->items[h + 1];
  psa_item_t res = {0};
  res.sym = SYM_E;

  if (k == 1 && r[0].sym == P_ID) { // E -> i
    res.node = r[0].node;
    res.type = r[0].type;
    res.line = r[0].line;

  } else if (k == 2 && r[0].sym == P_UN && r[1].sym == SYM_E) { // E -> +E | -E
    data_type_t t = r[1].type;
    ast_node_t *n = r[1].node;
    if (r[0].tok == TOKEN_SUB) {
      if (n->type == AST_LITERAL && n->data.literal.type == TYPE_INT) {
        n->data.literal.i_val = -n->data.literal.i_val; // zaporný literál
      } else if (n->type == AST_LITERAL &&
                 n->data.literal.type == TYPE_DOUBLE) {
        n->data.literal.d_val = -n->data.literal.d_val;
      } else {
        // AST nema unarni  => -E je 0 - E
        ast_node_t *zero = (t == TYPE_DOUBLE)
                               ? ast_create_literal_double(0.0, r[0].line)
                               : ast_create_literal_int(0, r[0].line);
        zero->return_type = (t == TYPE_DOUBLE) ? TYPE_DOUBLE : TYPE_INT;
        n = ast_create_binop(SUB, zero, n, r[0].line);
        n->return_type = t;
      }
    }
    res.node = n;
    res.type = t;
    res.line = r[0].line;

  } else if (k == 3 && r[0].sym == SYM_E && r[2].sym == SYM_E &&
             (r[1].sym == P_MUL || r[1].sym == P_ADD || r[1].sym == P_REL ||
              r[1].sym == P_EQ)) {
    binop_type_t bt = tok_to_binop(r[1].tok);
    if (bt == DIV && r[0].type == TYPE_INT && r[2].type == TYPE_INT)
      bt = IDIV;
    data_type_t rt = binop_result_type(bt, r[0].type, r[2].type);
    res.node = ast_create_binop(bt, r[0].node, r[2].node, r[1].line);
    if (res.node == NULL)
      return 99;
    res.node->return_type = rt;
    res.type = rt;
    res.line = r[1].line;

  } else if (k == 3 && r[0].sym == P_LP && r[1].sym == SYM_E &&
             r[2].sym == P_RP) { // E -> (E)
    res.node = r[1].node;
    res.type = r[1].type;
    res.line = r[1].line;

  } else {
    return 2;
  }

  stack->top = h; // odstrani pravou stranu i zarazku
  return st_push(stack, res);
}

/**
 * @brief Určí výsledný typ binární operace
 *
 * Relační operace vrací @c TYPE_BOOL, @c + nad řetězci řetězec, aritmetika
 * nad čísly Int/Double (s rozšířením Int na Double). Je-li některý operand
 * @c TYPE_ANY, výsledek je @c TYPE_ANY
 * @param op Operace
 * @param l Typ levého operandu
 * @param r Typ pravého operandu
 * @return Výsledný typ
 */
static data_type_t binop_result_type(binop_type_t op, data_type_t l,
                                     data_type_t r) {
  if (op >= EQ) {
    return TYPE_BOOL;
  }
  if (l == TYPE_ANY || r == TYPE_ANY) {
    return TYPE_ANY;
  }
  if (op == ADD && l == TYPE_STRING && r == TYPE_STRING) {
    return TYPE_STRING;
  }
  if ((l == TYPE_INT || l == TYPE_DOUBLE) &&
      (r == TYPE_INT || r == TYPE_DOUBLE)) {
    return (l == TYPE_DOUBLE || r == TYPE_DOUBLE) ? TYPE_DOUBLE : TYPE_INT;
  }

  return TYPE_ANY;
}

/**
 * @brief Vytvoří operand z aktuálního tokenu (literál nebo proměnná)
 *
 * Rozpoznává literály Int, Double, String a Unit (@c ()) a identifikátory
 * proměnných (ověřené v tabulce symbolů)
 * @param[out] it Vytvořená položka zásobníku
 * @param local_symtable Tabulka symbolů pro vyhledání proměnných
 * @retval 0 Úspěch
 * @retval 2 Nepovolený token nebo samotné podtržítko
 * @retval 3 Nedefinovaná proměnná (nebo identifikátor funkce)
 * @retval 99 Chyba alokace
 */
static int make_operand(psa_item_t *it, tree_t *local_symtable) {
  int line = current_token.line;
  switch (current_token.type) {
  case TOKEN_INT:
    it->node = ast_create_literal_int((int)current_token.int_val, line);
    it->type = TYPE_INT;
    break;
  case TOKEN_DOUBLE:
    it->node = ast_create_literal_double(current_token.double_val, line);
    it->type = TYPE_DOUBLE;
    break;
  case TOKEN_STRING:
    it->node = ast_create_literal_string(current_token.str_val, line);
    it->type = TYPE_STRING;
    break;
  case TOKEN_LPAREN: // literal () typu Unit
    next();          // ted jsem na )
    it->node = ast_create_literal_unit(line);
    it->type = TYPE_UNIT;
    break;
  case TOKEN_IDENTIFIER: {
    if (strcmp(current_token.str_val, "_") == 0) {
      return 2; // samotne podtrzitko neni identifikator
    }
    tdata_t *sym = symtable_lookup(local_symtable, current_token.str_val);
    if (sym == NULL || sym->type == FUNCTION) {
      return 3;
    }
    it->type = sym->data_type;
    it->node =
        ast_create_var(current_token.str_val, sym->data_type, LOCAL, line);
    if (it->node != NULL) {
      it->node->data.variable.id = sym->id;
    }
    break;
  }
  default:
    return 2;
  }
  if (it->node == NULL)
    return 99;
  it->node->return_type = it->type;
  it->line = line;
  return 0;
}

/**
 * @brief Uvolní zásobník a uzly AST, které na něm zůstaly
 * @param stack Zásobník
 * @param err_code Kód, který se vrátí
 * @return Hodnota @p err_code
 */
static int clean(psa_stack_t *stack, int err_code) {
  if (stack->items != NULL) {
    for (int i = 0; i < stack->top; i++) {
      ast_destroy_node(stack->items[i].node);
    }
    free(stack->items);
    stack->items = NULL;
  }
  return err_code;
}

/**
 * @brief Precedenční syntaktická analýza výrazu
 *
 * Zpracovává literály, proměnné, závorky, unární @c + @c - a binární operátory
 * @c * @c / @c + @c - @c < @c <= @c > @c >= @c == @c !=. Výraz končí prvním
 * tokenem, který do výrazu nepatří. Konce řádků jsou povoleny uvnitř závorek
 * a při @p in_parens, vždy s kontrolou odsazení pokračování. 
 * @param local_symtable Tabulka symbolů pro vyhledání proměnných
 * @param[out] out_type Typ výrazu
 * @param[out] out_node Kořen AST výrazu (může být NULL, pak se uzel uvolní)
 * @param in_parens Výraz je uvnitř závorek (např argument volání funkce)
 * @retval 0 Úspěch
 * @retval 2 Syntaktická chyba
 * @retval 3 Nedefinovaná proměnná
 * @retval 99 Interní chyba
 */
int parse_expression_ex(tree_t *local_symtable, data_type_t *out_type,
                        ast_node_t **out_node, bool in_parens) {

  psa_stack_t stack = {NULL, 0, 0};
  psa_item_t dollar = {0};
  dollar.sym = P_END;

  int res = st_push(&stack, dollar);
  if (res != 0)
    return res;

  int depth = 0;             // zavorky v tomto vyrazu
  bool prev_operand = false; // pokud je posledni operand symbol nebo )

  while (true) {
    int ti = st_top_terminal(&stack);
    int top_sym = stack.items[ti].sym;

    // EOL muze byt uvnitr zavorek nebo hned za operatorem
    if (current_token.type == TOKEN_EOL && (in_parens || depth > 0)) {
      next();
      // pokracovani musi byt odsazeno vic nez zacatek  radku
      if (current_token.type != TOKEN_EOL && current_token.type != TOKEN_EOF &&
          current_token.col <= parser_line_indent) {
        return clean(&stack, 2);
      }
      continue;
    }

    int in;
    switch (current_token.type) {
    case TOKEN_INT:
    case TOKEN_DOUBLE:
    case TOKEN_STRING:
      in = P_ID;
      break;
    case TOKEN_IDENTIFIER:
      // identifikator + (
      if (peek_token().type == TOKEN_LPAREN) {
        return clean(&stack, 2);
      }
      in = P_ID;
      break;
    case TOKEN_LPAREN:
      in = (!prev_operand && peek_token().type == TOKEN_RPAREN) ? P_ID : P_LP;
      break;
    case TOKEN_RPAREN:
      in = (depth == 0) ? P_END : P_RP;
      break;
    case TOKEN_ADD:
    case TOKEN_SUB:
      in = prev_operand ? P_ADD : P_UN;
      break;
    case TOKEN_MUL:
    case TOKEN_DIV:
      in = P_MUL;
      break;
    case TOKEN_LESS:
    case TOKEN_LESS_EQUAL:
    case TOKEN_GREATER:
    case TOKEN_GREATER_EQUAL:
      in = P_REL;
      break;
    case TOKEN_EQ:
    case TOKEN_NEQ:
      in = P_EQ;
      break;
    default:
      in = P_END;
      break; // vse ostatni ukoncuje
    }

    // dva operandy za sebou nebo operator bez leveho operandu
    if ((in == P_ID || in == P_LP) && prev_operand) {
      return clean(&stack, 2);
    }
    if ((in == P_MUL || in == P_REL || in == P_EQ || in == P_RP) &&
        !prev_operand) {
      return clean(&stack, 2);
    }

    char act = PSA_TABLE[top_sym][in];

    if (act == 'F') { // konec vyrazu
      if (stack.top == 2 && stack.items[1].sym == SYM_E) {
        break;
      }
      return clean(&stack, 2); // prazdny nebo neuplny vyraz
    }
    if (act == ' ') {
      return clean(&stack, 2);
    }

    if (act == '>') {
      res = psa_reduce(&stack);
      if (res != 0) {
        return clean(&stack, res);
      }
      continue;
    }

    // <, =, :,
    psa_item_t it = {0};
    it.sym = in;
    it.tok = current_token.type;
    it.line = current_token.line;

    if (in == P_ID) {
      res = make_operand(&it, local_symtable);
      if (res != 0) {
        return clean(&stack, res);
      }
    }
    if (act == '<') {
      res = st_insert_handle_after(&stack, ti);
      if (res != 0) {
        return clean(&stack, res);
      }
    }
    res = st_push(&stack, it);
    if (res != 0) {
      return clean(&stack, res);
    }

    if (in == P_LP) {
      depth++;
    }
    if (in == P_RP) {
      depth--;
    }
    prev_operand = (in == P_ID || in == P_RP);
    next();
  }

  // predani uzlu
  if (out_type != NULL) {
    *out_type = stack.items[1].type;
  }
  if (out_node != NULL) {
    *out_node = stack.items[1].node;
  } else {
    ast_destroy_node(stack.items[1].node);
  }

  stack.items[1].node = NULL;
  return clean(&stack, 0);
}