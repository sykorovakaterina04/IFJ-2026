#include "ast.h"

ast_tree_t *ast_init() {
  ast_tree_t *ast = calloc(1, sizeof(ast_tree_t));
  if (ast == NULL) {
    fprintf(stderr, "%s", "chyba pri alokaci stromu");
    exit(99);
  }
  return ast;
}

static ast_node_t *ast_create_node(ast_type_t type) {
  ast_node_t *new_node = calloc(1, sizeof(ast_node_t));
  if (new_node == NULL) {
    fprintf(stderr, "%s", "chyba pri alokaci stromu");
    exit(99);
  }
  new_node->type = type;
  return new_node;
}

ast_node_t *ast_create_binop(binop_type_t type, ast_node_t *left,
                             ast_node_t *right, int line) {
  ast_node_t *binop = ast_create_node(AST_BINOP);

  binop->data.binop.type = type;
  binop->data.binop.left = left;
  binop->data.binop.right = right;
  binop->line = line;

  return binop;
}

ast_node_t *ast_create_assign(ast_node_t *source, ast_node_t *dest, int line) {
  ast_node_t *assign = ast_create_node(AST_ASSIGN);

  assign->data.assign.source = source;
  assign->data.assign.dest = dest;
  assign->line = line;

  return assign;
}

ast_node_t *ast_create_block(tree_t *tree, ast_node_t *first_line) {
  ast_node_t *block = ast_create_node(AST_BLOCK);

  block->data.block.first_line = first_line;
  block->data.block.symtable = tree;

  return block;
}

ast_node_t *ast_create_if(ast_node_t *cond, ast_node_t *body,
                          ast_node_t *else_body) {
  ast_node_t *conditional = ast_create_node(AST_IF);

  conditional->data.condicionals.cond = cond;
  conditional->data.condicionals.body = body;
  conditional->data.condicionals.else_body = else_body;

  return conditional;
}

ast_node_t *ast_create_var(char *name, data_type_t type, frame_t frame,
                           int line) {

  if (name == NULL)
    return NULL;

  ast_node_t *var = ast_create_node(AST_VAR);

  var->data.variable.name = malloc(strlen(name) + 1);
  if (var->data.variable.name == NULL) {
    exit(99);
  }

  strcpy(var->data.variable.name, name);
  var->data.variable.frame = frame;
  var->data.variable.type = type;
  var->line = line;

  return var;
}

ast_node_t *ast_create_var_def(char *name, data_type_t type, int line) {

  if (name == NULL)
    return NULL;

  ast_node_t *var_def = ast_create_node(AST_VAR_DEF);

  var_def->data.variable.name = malloc(strlen(name) + 1);
  if (var_def->data.variable.name == NULL) {
    exit(99);
  }

  strcpy(var_def->data.variable.name, name);
  var_def->data.variable.type = type;
  var_def->return_type = type;
  var_def->line = line;

  return var_def;
}

ast_node_t *ast_create_while(ast_node_t *cond, ast_node_t *body) {
  ast_node_t *while_loop = ast_create_node(AST_WHILE);

  while_loop->data.condicionals.cond = cond;
  while_loop->data.condicionals.body = body;

  return while_loop;
}

ast_node_t *ast_create_func_def(char *name, ast_node_t *args, ast_node_t *body,
                                int line) {

  if (name == NULL)
    return NULL;

  ast_node_t *func_def = ast_create_node(AST_FUNC_DEF);

  func_def->data.func.name = malloc(strlen(name) + 1);
  if (func_def->data.func.name == NULL) {
    exit(99);
  }

  strcpy(func_def->data.func.name, name);
  func_def->data.func.args = args;
  func_def->data.func.body = body;
  func_def->line = line;

  return func_def;
}

ast_node_t *ast_create_func_call(char *name, ast_node_t *args, int line) {

  if (name == NULL)
    return NULL;

  ast_node_t *func_call = ast_create_node(AST_CALL);

  func_call->data.func.name = malloc(strlen(name) + 1);
  if (func_call->data.func.name == NULL) {
    exit(99);
  }
  strcpy(func_call->data.func.name, name);
  func_call->data.func.args = args;
  func_call->line = line;

  return func_call;
}

ast_node_t *ast_create_literal_int(int value, int line) {
  ast_node_t *literal_int = ast_create_node(AST_LITERAL);

  literal_int->data.literal.i_val = value;
  literal_int->data.literal.type = TYPE_INT;
  literal_int->return_type = TYPE_INT;
  literal_int->line = line;

  return literal_int;
}

ast_node_t *ast_create_literal_double(double value, int line) {
  ast_node_t *literal_double = ast_create_node(AST_LITERAL);

  literal_double->data.literal.d_val = value;
  literal_double->data.literal.type = TYPE_DOUBLE;
  literal_double->return_type = TYPE_DOUBLE;
  literal_double->line = line;

  return literal_double;
}

ast_node_t *ast_create_literal_string(char *value, int line) {

  if (value == NULL)
    return NULL;

  ast_node_t *literal_string = ast_create_node(AST_LITERAL);

  literal_string->data.literal.string_val = malloc(strlen(value) + 1);
  if (literal_string->data.literal.string_val == NULL) {
    exit(99);
  }

  strcpy(literal_string->data.literal.string_val, value);
  literal_string->data.literal.type = TYPE_STRING;
  literal_string->return_type = TYPE_STRING;
  literal_string->line = line;

  return literal_string;
}

ast_node_t *ast_create_literal_unit(int line) {
  ast_node_t *literal_unit = ast_create_node(AST_LITERAL);

  literal_unit->data.literal.type = TYPE_UNIT;
  literal_unit->return_type = TYPE_UNIT;
  literal_unit->line = line;

  return literal_unit;
}

ast_node_t *ast_create_return(ast_node_t *var, ast_node_t *func_def, int line) {
  ast_node_t *ast_return = ast_create_node(AST_RETURN);

  ast_return->data.node_return.var = var;
  ast_return->data.node_return.func_def = func_def;
  ast_return->line = line;

  return ast_return;
}

void ast_destroy_node(ast_node_t *node) {

  if (node == NULL) {
    return;
  }

  switch (node->type) {
  case AST_ASSIGN:
    ast_destroy_node(node->data.assign.dest);
    ast_destroy_node(node->data.assign.source);
    break;

  case AST_BINOP:
    ast_destroy_node(node->data.binop.left);
    ast_destroy_node(node->data.binop.right);
    break;

  case AST_BLOCK:
    ast_destroy_node(node->data.block.first_line);
    symtable_destroy_tree(node->data.block.symtable);
    break;

  case AST_IF:
    ast_destroy_node(node->data.condicionals.cond);
    ast_destroy_node(node->data.condicionals.body);
    ast_destroy_node(node->data.condicionals.else_body);
    break;

  case AST_WHILE:
    ast_destroy_node(node->data.condicionals.cond);
    ast_destroy_node(node->data.condicionals.body);
    break;

  case AST_FUNC_DEF:
    ast_destroy_node(node->data.func.args);
    ast_destroy_node(node->data.func.body);
    free(node->data.func.name);
    break;

  case AST_CALL:
    ast_destroy_node(node->data.func.args);
    free(node->data.func.name);
    break;

  case AST_VAR_DEF:
    free(node->data.variable.name);
    break;

  case AST_VAR:
    free(node->data.variable.name);
    break;

  case AST_LITERAL:
    if (node->data.literal.type == TYPE_STRING) {
      free(node->data.literal.string_val);
    }
    break;

  case AST_RETURN:
    ast_destroy_node(node->data.node_return.var);
    break;
  }
  ast_destroy_node(node->next);
  free(node);
}

void ast_destroy_tree(ast_tree_t *tree) {
  if (tree == NULL) {
    return;
  }

  ast_destroy_node(tree->root);
  free(tree);
}