/**
 *
 * @file semantic.c
 * @author Daniel Burda (xburdad00), Ema Osmerova (xosmere00)
 * @brief projekt IFJ a IAL
 * @version 0.1
 * @date 2026-11-27
 *
 */
#include "semantic.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief funkce, ktera projde vsechny uzly v ast a zkontroluje je
 *
 * @param tree
 * @param node
 * @param local_symtable
 * @param error
 */
void sem_check_node(ast_tree_t *tree, ast_node_t *node, tree_t *local_symtable,
                    int *error);
/**
 * @brief pomocna funkce pro overeni, jestli se jedna o ciselny datovy typ
 *
 * @param type
 * @return true
 * @return false
 */
bool is_numeric(data_type_t type) {
  return (type == TYPE_INT || type == TYPE_DOUBLE);
}

/**
 * @brief pomocna funkce pro overeni, jestli se jedna o relacni operator
 *
 * @param type
 * @return true
 * @return false
 */
bool is_relational_op(binop_type_t type) {
  return (type == EQ || type == NEQ || type == LT || type == GT ||
          type == LEQ || type == GEQ);
}

/**
 * @brief funkce zajistujici konverzi typu Int na typ Double
 *
 * @param operand
 */
void convert_int_to_double(ast_node_t *operand) {
  if (operand == NULL) {
    return;
  }
  if (operand->type == AST_VAR && operand->data.variable.type == TYPE_INT) {
    operand->data.variable.conversion_type = TYPE_DOUBLE;
  } else if (operand->type == AST_LITERAL &&
             operand->data.literal.type == TYPE_INT) {
    operand->data.literal.conversion_type = TYPE_DOUBLE;
  }
}

/**
 * @brief kontroluje spravnost binarnich operaci a odvozuje jejich typ
 *
 * @param tree
 * @param node
 * @param local_symtable
 * @param error
 */
void check_binop(ast_tree_t *tree, ast_node_t *node, tree_t *local_symtable,
                 int *error) {
  if (node == NULL || error == NULL || *error != 0) {
    return;
  }

  // kontrola leveho a praveho podvyrazu
  sem_check_node(tree, node->data.binop.left, local_symtable, error);
  sem_check_node(tree, node->data.binop.right, local_symtable, error);
  if (*error != 0) {
    return;
  }

  // nacteni odvozenych navratovych typu obou operandu
  data_type_t left_type = node->data.binop.left->return_type;
  data_type_t right_type = node->data.binop.right->return_type;

  if (node->data.binop.type == ADD) {
    // scitani celych cisel, smisene scitani s konverzi Int na Double
    if (left_type == TYPE_INT && right_type == TYPE_INT) {
      node->return_type = TYPE_INT;
    } else if (is_numeric(left_type) && is_numeric(right_type)) {
      node->return_type = TYPE_DOUBLE;
      convert_int_to_double(node->data.binop.left);
      convert_int_to_double(node->data.binop.right);
    } else if (left_type == TYPE_STRING && right_type == TYPE_STRING) {
      node->return_type = TYPE_STRING;
    } else {
      *error = 6;
    }
  } else if (node->data.binop.type == SUB || node->data.binop.type == MUL ||
             node->data.binop.type == DIV) {
    // aritmeticke operace nad Int s pripadnou konverzi Int na Double
    if (left_type == TYPE_INT && right_type == TYPE_INT) {
      node->return_type = TYPE_INT;
      if (node->data.binop.type == DIV) {
        node->data.binop.type = IDIV;
      }
    } else if (is_numeric(left_type) && is_numeric(right_type)) {
      node->return_type = TYPE_DOUBLE;
      convert_int_to_double(node->data.binop.left);
      convert_int_to_double(node->data.binop.right);
    } else {
      *error = 6;
    }
  } else if (node->data.binop.type == IDIV) {
    // celociselne deleni, oba operandy musi byt typu Int
    if (left_type == TYPE_INT && right_type == TYPE_INT) {
      node->return_type = TYPE_INT;
    } else {
      *error = 6;
    }
  } else if (is_relational_op(node->data.binop.type)) {
    // relacni operatory vraci Bool
    if (is_numeric(left_type) && is_numeric(right_type)) {
      convert_int_to_double(node->data.binop.left);
      convert_int_to_double(node->data.binop.right);
      node->return_type = TYPE_BOOL;
    } else if (left_type == TYPE_STRING && right_type == TYPE_STRING) {
      node->return_type = TYPE_BOOL;
    } else {
      *error = 6;
    }
  } else {
    *error = 6;
  }
}

/**
 * @brief funkce, ktera kontroluje spravnost pristupu k promenne (AST_VAR)
 *
 * @param node
 * @param local_symtable
 * @param error
 */
void check_var(ast_node_t *node, tree_t *local_symtable, int *error) {
  if (node == NULL || error == NULL || *error != 0) {
    return;
  }

  // chybejici nazev promenne signalizuje chybny uzel AST
  char *var_name = node->data.variable.name;
  if (var_name == NULL) {
    *error = 99;
    return;
  }

  // vyhledani symbolu a odmitnuti nazvu patriciho funkci
  tdata_t *data = symtable_lookup(local_symtable, var_name);
  if (data == NULL || data->type == FUNCTION) {
    *error = 3;
    return;
  }

  // zapis datoveho typu do uzlu promenne a jeho navratoveho typu
  node->data.variable.type = data->data_type;
  node->return_type = data->data_type;

  // neodvozeny TYPE_ANY u promenne konci chybou
  if (node->return_type == TYPE_ANY) {
    *error = 6;
  }
}

/**
 * @brief funkce, ktera rekurzivne prohledava strom, aby nasla uzel podle jmena
 * a typu
 *
 * @param node
 * @param type typ uzlu, ktery hledame
 * @param func_name nazev funkce, kterou hledame
 * @param var_name nazev promenne, kterou hledame
 * @return ast_node_t*
 */
ast_node_t *find_node(ast_node_t *node, ast_type_t type, char *func_name,
                      char *var_name) {
  // kontrola parametru
  if (!node) {
    return NULL;
  }

  // pokud typ souhlasi, nebo podle nej nevyhledavame
  if (type == AST_NO_TYPE || node->type == type) {

    // pokud hledame podle jmena funkce
    if (func_name != NULL) {

      // pokud typ souhlasi na funkci a shoduji se jmena, vracime uzel
      if ((node->type == AST_FUNC_DEF || node->type == AST_CALL) &&
          node->data.func.name != NULL &&
          strcmp(node->data.func.name, func_name) == 0) {
        return node;
      }

      // pokud hledame podle jmena promenne
    } else if (var_name != NULL) {

      // pokud odpovida typ na promennou a shoduji se jmena, vracime uzel
      if ((node->type == AST_VAR_DEF || node->type == AST_VAR) &&
          node->data.variable.name != NULL &&
          strcmp(node->data.variable.name, var_name) == 0) {
        return node;
      }

      // pokud hledame jen podle typu
    } else if (type != AST_NO_TYPE) {

      // kdyz se typy rovnaji, vracime uzel
      if (node->type == type) {
        return node;
      }
    }
  }

  // nastaveni ukazatele na uzel, ktery budeme vracet
  ast_node_t *target_node = NULL;

  // pruchod stromem podle typu uzlu, na kterem zrovna jsme
  switch (node->type) {
  case AST_ASSIGN:
    target_node = find_node(node->data.assign.dest, type, func_name, var_name);
    if (target_node != NULL)
      return target_node;
    target_node =
        find_node(node->data.assign.source, type, func_name, var_name);
    break;

  case AST_BINOP:
    target_node = find_node(node->data.binop.left, type, func_name, var_name);
    if (target_node != NULL)
      return target_node;
    target_node = find_node(node->data.binop.right, type, func_name, var_name);
    break;

  case AST_BLOCK:
    target_node =
        find_node(node->data.block.first_line, type, func_name, var_name);
    break;

  case AST_IF:
    target_node =
        find_node(node->data.condicionals.cond, type, func_name, var_name);
    if (target_node != NULL)
      return target_node;
    target_node =
        find_node(node->data.condicionals.body, type, func_name, var_name);
    if (target_node != NULL)
      return target_node;
    target_node =
        find_node(node->data.condicionals.else_body, type, func_name, var_name);
    break;

  case AST_WHILE:
    target_node =
        find_node(node->data.condicionals.cond, type, func_name, var_name);
    if (target_node != NULL)
      return target_node;
    target_node =
        find_node(node->data.condicionals.body, type, func_name, var_name);
    break;

  case AST_FUNC_DEF:
    target_node = find_node(node->data.func.args, type, func_name, var_name);
    if (target_node != NULL)
      return target_node;
    target_node = find_node(node->data.func.body, type, func_name, var_name);
    break;

  case AST_CALL:
    target_node = find_node(node->data.func.args, type, func_name, var_name);
    break;

  case AST_VAR_DEF:
    break;

  case AST_VAR:
    break;

  case AST_LITERAL:
    break;

  case AST_RETURN:
    target_node =
        find_node(node->data.node_return.var, type, func_name, var_name);
    break;

  case AST_NO_TYPE:
    break;
  }
  if (target_node != NULL) {
    return target_node;
  }

  return find_node(node->next, type, func_name, var_name);
}

/**
 * @brief zkontroluje, jestli if na konci funkce vraci unit nebo ne
 *
 * @param node
 * @return true
 * @return false
 */
bool branch_unit(ast_node_t *node) {

  // kontrola parametru a prirazeni ukazatele na prvni radek funkce
  if (node != NULL && node->type == AST_BLOCK) {
    node = node->data.block.first_line;
  }

  // pokud je telo prazdne, vraci true
  if (node == NULL) {
    return true;
  }

  // posunuti ukazatele na posledni vytaz
  while (node->next != NULL) {
    node = node->next;
  }

  switch (node->type) {

  // pokud je vyraz if, rekurzivne se vnori
  case AST_IF:
    return branch_unit(node->data.condicionals.body) &&
           branch_unit(node->data.condicionals.else_body);

  // pokud je vytaz jakykoliv z techto typu, vraci true
  case AST_ASSIGN:
  case AST_VAR_DEF:
  case AST_WHILE:
    return true;

  // pokud je vyraz jakykoliv z techto typu, kontroluje jestli se typy rovnaji
  case AST_CALL:
  case AST_LITERAL:
    return (node->return_type == TYPE_UNIT);

  // jinak vraci false
  default:
    return false;
  }
}

/**
 * @brief zkontroluje if vetve, kdyz je to posledni prikaz v definici funkce,
 * ktera nema urceny typ
 *
 * @param node
 * @param expected
 * @return true
 * @return false
 */
bool branch_return(ast_node_t *node, data_type_t expected) {

  // kontrola parametru
  if (node != NULL && node->type == AST_BLOCK) {
    node = node->data.block.first_line;
  }

  if (node == NULL) {
    return false;
  }
  // dostani se na posledni radek
  while (node->next != NULL) {
    node = node->next;
  }
  // pokud je tam uzel return
  if (node->type == AST_RETURN) {
    if (node->data.node_return.var == NULL) {
      return false;
    }

    // ulozeni toho, co vraci vyraz
    data_type_t type = node->data.node_return.var->return_type;

    // pokud se rovnaji nebo se ocekava double a vraci se int, vraci true
    return (type == expected) || (expected == TYPE_DOUBLE && type == TYPE_INT);
  }

  // vnoreni do dalsich vetvi if
  if (node->type == AST_IF) {
    return branch_return(node->data.condicionals.body, expected) &&
           branch_return(node->data.condicionals.else_body, expected);
  }
  return false;
}

/**
 * @brief funkce, ktera overi kompatibilitu typu u funkci
 *
 * @param node
 * @param local_symtable
 * @param error
 */
void check_func_2(ast_node_t *node, tree_t *local_symtable, int *error) {

  // kontrola parametru
  if (node == NULL)
    return;

  // ukazatel na prvni radek definice funkce
  ast_node_t *first_line = node->data.func.body->data.block.first_line;

  // osetreni, pokud je funkce prazdna
  if (first_line == NULL) {
    if (node->return_type != TYPE_UNIT && node->return_type != TYPE_ANY) {
      *error = 6;
    } else {
      node->return_type = TYPE_UNIT;
      tdata_t *data = symtable_lookup(local_symtable, node->data.func.name);
      if (data != NULL) {
        data->data_type = TYPE_UNIT;
      }
    }
    return;
  }

  // deklarace ukazatele na posledni radek a promenne datoveho typu
  ast_node_t *last_line = first_line;
  data_type_t type = TYPE_ANY;

  // nastaveni ukazatele na posledni radek
  while (last_line->next != NULL) {
    last_line = last_line->next;
  }

  // pokud funkce nema urceny return_type
  if (node->return_type == TYPE_ANY) {

    // pokud je posledni radek nejaky z techto typu, return type bude unit
    if (last_line->type == AST_ASSIGN || last_line->type == AST_FUNC_DEF ||
        last_line->type == AST_VAR_DEF || last_line->type == AST_WHILE) {
      type = TYPE_UNIT;
    }
    // pokud je posledni vyraz IF, nastavi se budto typ unit nebo se vrati
    // chyba
    if (last_line->type == AST_IF) {
      if (branch_unit(last_line)) {
        type = TYPE_UNIT;
      } else {
        *error = 6;
      }
    }

    // pokud je na konci jakykoliv z techto typu, nastavi se return type
    // funkce na return type techto vyrazu
    if (last_line->type == AST_VAR || last_line->type == AST_LITERAL ||
        last_line->type == AST_BINOP || last_line->type == AST_CALL) {
      type = last_line->return_type;
    }

    // pokud je type porad nerozhodny nebo je bool, vraci se chyba
    if (type == TYPE_ANY || type == TYPE_BOOL) {
      *error = 6;
    }

    // nastaveni return typu funkce
    node->return_type = type;

    // zmena zapisu v tabulce symbolu
    tdata_t *data = symtable_lookup(local_symtable, node->data.func.name);
    if (data != NULL) {
      data->data_type = type;
    }

    // kdyz je typ funkce int, string nebo double
  } else if (node->return_type != TYPE_UNIT) {

    // kdyz je posledni radek typu return
    if (last_line->type == AST_RETURN) {

      // pokud neexistuje vraceny vyraz, vrati chybu
      if (last_line->data.node_return.var == NULL) {
        *error = 6;
        return;
      }

      // ulozi do type return type vraceneho vyrazu
      type = last_line->data.node_return.var->return_type;

      // pokud je typ funkce double a typ co se vraci int, provede
      // konverzi
      if (node->return_type == TYPE_DOUBLE && type == TYPE_INT) {
        convert_int_to_double(last_line->data.node_return.var);

        // pokud se jinak typy lisi, vraci chybu
      } else if (type != node->return_type) {
        *error = 6;
        return;
      }

    }

    // pokud je na poslednim radku jakykoliv z techto typu
    else if (last_line->type == AST_VAR || last_line->type == AST_LITERAL ||
             last_line->type == AST_BINOP || last_line->type == AST_CALL) {
      // pokud je return funkce double a vraceny vyraz je int, provede
      // konverzi
      if (node->return_type == TYPE_DOUBLE &&
          last_line->return_type == TYPE_INT) {
        convert_int_to_double(last_line);
        // pokud se typy nerovnaji, vraci chybu
      } else if (last_line->return_type != node->return_type) {
        *error = 6;
        return;
      }
    }

    // pokud je posledni vyraz if a returny ve vetvich nevraci to, co by
    // meli, vraci chybu
    else if (last_line->type == AST_IF) {
      if (!branch_return(last_line, node->return_type)) {
        *error = 6;
        return;
      }
    }

    // kdyz se stane cokoliv jineho, vraci chybu
    else {
      *error = 6;
      return;
    }
  }
}

/**
 * @brief kontroluje, jestli je funkce rekurzivni nebo obsahuje return, pokud
 * nema uvedeny navratovy typ
 *
 * @param node
 * @param error
 */
void check_func_1(ast_node_t *node, int *error) {

  // kontrola parametru
  if (node == NULL || *error != 0) {
    return;
  }

  // zjisteni, jestli je funkce rekurzivni
  bool is_recursive = (find_node(node->data.func.body, AST_CALL,
                                 node->data.func.name, NULL) != NULL);

  // zjisteni, jestli ma funkce return
  bool has_return =
      (find_node(node->data.func.body, AST_RETURN, NULL, NULL) != NULL);

  // jestli nema uvedeny typ v deklaraci a je rekurzivni, vrati chybu
  if (node->return_type == TYPE_ANY && (is_recursive || has_return)) {
    *error = 6;
    return;
  }
}

/**
 * @brief funkce, ktera kontroluje spravnost vetveni if
 *
 * @param tree
 * @param node
 * @param local_symtable
 * @param error
 */
void check_if(ast_tree_t *tree, ast_node_t *node, tree_t *local_symtable,
              int *error) {
  if (node == NULL || error == NULL || *error != 0) {
    return;
  }

  if (node->data.condicionals.cond == NULL) {
    *error = 99;
    return;
  }

  // kontrola podminky a overeni navratoveho typu Bool
  sem_check_node(tree, node->data.condicionals.cond, local_symtable, error);
  if (*error != 0) {
    return;
  }
  if (node->data.condicionals.cond->return_type != TYPE_BOOL) {
    *error = 6;
    return;
  }

  // kontrola hlavni vetve
  if (node->data.condicionals.body == NULL) {
    *error = 99;
    return;
  } else {
    sem_check_node(tree, node->data.condicionals.body, local_symtable, error);
  }
  if (*error != 0) {
    return;
  }

  // kontrola volitelne vetve else
  if (*error == 0) {
    if (node->data.condicionals.else_body != NULL) {
      sem_check_node(tree, node->data.condicionals.else_body, local_symtable,
                     error);
    }
  }
}

/**
 * @brief funkce, ktera kontroluje spravnost cyklu while
 *
 * @param tree
 * @param node
 * @param local_symtable
 * @param error
 */
void check_while(ast_tree_t *tree, ast_node_t *node, tree_t *local_symtable,
                 int *error) {
  if (node == NULL || error == NULL || *error != 0) {
    return;
  }

  if (node->data.condicionals.cond == NULL) {
    *error = 99;
    return;
  }

  // kontrola podminky a overeni typu Bool
  sem_check_node(tree, node->data.condicionals.cond, local_symtable, error);
  if (*error != 0) {
    return;
  }
  if (node->data.condicionals.cond->return_type != TYPE_BOOL) {
    *error = 6;
    return;
  }

  // kontrola tela cyklu
  if (node->data.condicionals.body == NULL) {
    *error = 99;
    return;
  } else {
    sem_check_node(tree, node->data.condicionals.body, local_symtable, error);
  }
  if (*error != 0) {
    return;
  }
}

/**
 * @brief funkce, ktera kontroluje spravnost prirazeni
 *
 * @param tree
 * @param node
 * @param local_symtable
 * @param error
 */
void check_assign(ast_tree_t *tree, ast_node_t *node, tree_t *local_symtable,
                  int *error) {
  if (node == NULL || error == NULL || *error != 0) {
    return;
  }

  ast_node_t *source = node->data.assign.source;
  ast_node_t *dest = node->data.assign.dest;
  if (source == NULL || dest == NULL) {
    *error = 99;
    return;
  }

  sem_check_node(tree, source, local_symtable, error);
  if (*error != 0) {
    return;
  }
  sem_check_node(tree, dest, local_symtable, error);
  if (*error != 0) {
    return;
  }

  if (dest->type != AST_VAR) {
    *error = 99;
    return;
  } else {
    tdata_t *var = symtable_lookup(local_symtable, dest->data.variable.name);
    if (var == NULL) {
      *error = 3;
      return;
    }
  }

  data_type_t dest_type = dest->return_type;
  data_type_t source_type = source->return_type;

  if (dest_type == source_type || source_type == TYPE_ANY) {
    return;
  } else if (source_type == TYPE_INT && dest_type == TYPE_DOUBLE) {
    convert_int_to_double(source);
  } else {
    *error = 6;
  }
}

/**
 * @brief funkce, ktera kontroluje volani funkce
 *
 * @param tree
 * @param node
 * @param local_symtable
 * @param error
 */
void check_call(ast_tree_t *tree, ast_node_t *node, tree_t *local_symtable,
                int *error) {
  if (node == NULL || error == NULL || *error != 0) {
    return;
  }

  char *func_name = node->data.func.name;
  if (func_name == NULL) {
    *error = 99;
    return;
  }

  // kontrola existence funkce
  tdata_t *func_sym = symtable_lookup(local_symtable, func_name);
  if (func_sym == NULL || func_sym->type != FUNCTION) {
    *error = 3;
    return;
  }

  // vyhledani funkce v AST a jeji jednorazova kontrola pri prvnim volani
  ast_node_t *func_def = find_node(tree->root, AST_FUNC_DEF, func_name, NULL);
  if (func_def != NULL && func_def->data.func.is_checked == false) {
    func_def->data.func.is_checked = true;
    // zakladni kontrola hlavicky a navratoveho typu funkce
    check_func_1(func_def, error);
    if (*error != 0) {
      return;
    }
    // kontrola argumentu, tela funkce a odvozeni/overeni navratoveho typu v
    // jeji tabulce symbolu
    if (func_def->data.func.body != NULL &&
        func_def->data.func.body->data.block.symtable != NULL) {
      tree_t *func_symtable = func_def->data.func.body->data.block.symtable;
      sem_check_node(tree, func_def->data.func.args, func_symtable, error);
      sem_check_node(tree, func_def->data.func.body, func_symtable, error);
      check_func_2(func_def, func_symtable, error);
    } else {
      // interni chyba AST
      *error = 99;
      return;
    }
    if (*error != 0) {
      return;
    }
  }

  // prochazim argumenty
  ast_node_t *arg = node->data.func.args;
  int arg_count = 0;
  while (arg != NULL) {
    sem_check_node(tree, arg, local_symtable, error);
    if (*error != 0) {
      return;
    }
    arg_count++;
    arg = arg->next;
  }

  // kontrola poctu argumentu
  if (arg_count != func_sym->param_count) {
    *error = 5;
    return;
  }

  // kontrola typu a konverze
  arg = node->data.func.args;
  for (int i = 0; i < arg_count && arg != NULL; i++) {
    data_type_t expected = func_sym->param_types[i];
    data_type_t actual = arg->return_type;

    if (expected == TYPE_DOUBLE && actual == TYPE_INT) {
      convert_int_to_double(arg);
    } else if (expected != actual && expected != TYPE_ANY &&
               actual != TYPE_ANY) {
      *error = 5;
      return;
    }

    arg = arg->next;
  }

  node->return_type = func_sym->data_type;
}

/**
 * @brief funkce, ktera kontroluje uzel return
 *
 * @param tree
 * @param node
 * @param local_symtable
 * @param error
 */
void check_return(ast_tree_t *tree, ast_node_t *node, tree_t *local_symtable,
                  int *error) {
  // kontrola parametru
  if (node == NULL)
    return;

  // funkce, ve ktere se return nachazi
  ast_node_t *current_func = node->data.node_return.func_def;

  // pokud se nachazi mimo funkci, vrati se chyba
  if (current_func == NULL) {
    *error = 10;
    return;
  }

  // vyraz, ktery return vraci
  ast_node_t *return_var = node->data.node_return.var;

  // jaky typ ma vracet funkce, ve ktere se return nachazi
  data_type_t func_return = current_func->return_type;

  // pokud typ vracene hodnoty neni uvedeny, vraci chybu
  if (func_return == TYPE_ANY) {
    *error = 6;
    return;
  }

  // pokud funkce vraci unit a return_var neni NULL, vraci se chyba
  if (func_return == TYPE_UNIT) {
    if (return_var != NULL) {
      *error = 6;
    }
    return;
  }

  // pokud se nevraci unit a neexistuje vracejici se vyraz, vraci se chyba
  if (return_var == NULL) {
    *error = 6;
    return;
  }

  // semanticka kontrola vraceneho vyrazu
  sem_check_node(tree, return_var, local_symtable, error);
  if (*error != 0) {
    return;
  }

  // pokud je vraceny vyraz int a datovy typ funkce je double, provede
  // konverzi
  if (func_return == TYPE_DOUBLE && return_var->return_type == TYPE_INT) {
    convert_int_to_double(return_var);
    // pokud jsou to jakekoliv jeno datove typy, co se nerovnaji, vraci
    // chybu
  } else if (return_var->return_type != func_return) {
    *error = 6;
  }
  return;
}

/**
 * @brief funkce, ktera projde vsechny uzly v ast a zkontroluje je
 *
 * @param tree
 * @param node
 * @param local_symtable
 * @param error
 */
void sem_check_node(ast_tree_t *tree, ast_node_t *node, tree_t *local_symtable,
                    int *error) {

  // kontrola parametru
  if (node == NULL || *error != 0) {
    return;
  }

  // projiti a kontrola podle typu uzlu
  switch (node->type) {
  case AST_ASSIGN:
    check_assign(tree, node, local_symtable, error);
    break;

  case AST_BINOP:
    check_binop(tree, node, local_symtable, error);
    break;

  case AST_BLOCK: {
    // zmena tabulky na tabulku urciteho ramce
    tree_t *block_symtable = node->data.block.symtable;
    sem_check_node(tree, node->data.block.first_line, block_symtable, error);
    break;
  }

  case AST_IF:
    check_if(tree, node, local_symtable, error);
    break;

  case AST_WHILE:
    check_while(tree, node, local_symtable, error);
    break;

  case AST_FUNC_DEF:
    if (node->data.func.is_checked == false) {
      check_func_1(node, error);
      sem_check_node(tree, node->data.func.args, local_symtable, error);
      sem_check_node(tree, node->data.func.body, local_symtable, error);
      check_func_2(node, local_symtable, error);
    }
    break;

  case AST_CALL:
    check_call(tree, node, local_symtable, error);
    break;

  case AST_VAR_DEF:
    break;

  case AST_VAR:
    check_var(node, local_symtable, error);
    break;

  case AST_LITERAL:

    break;

  case AST_RETURN:
    check_return(tree, node, local_symtable, error);
    break;

  case AST_NO_TYPE:
    break;
  }
  sem_check_node(tree, node->next, local_symtable, error);
}

/**
 * @brief verejna funkce pro kontrolu ast stromu
 *
 * @param tree
 * @return int
 */
int sem_check(ast_tree_t *tree) {
  int error = 0;

  sem_check_node(tree, tree->root, tree->global_symtable, &error);

  return error;
}
