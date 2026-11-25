#include "automat.h"
#include "codegen.h"
#include "parser.h"
#include "semantic.h"

int main(void) {
  ast_tree_t *ast = NULL;

  int error = parse(&ast);
  if (error) {
    return error;
  }

  error = sem_check(ast);
  if (error) {
    ast_destroy_tree(ast);
    return error;
  }

  generuj_kod(ast);

  ast_destroy_tree(ast);

  return 0;
}