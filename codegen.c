#include "codegen.h"
//#include "ast.h"
#include <stdio.h>

void pruchod_stromem(ast_node_t *node){

    if(node == NULL){
        return;
    }
    switch (node->type){
        case AST_FUNC_DEF:
            //TODO
            break;
        case AST_VAR_DEF:
            //TODO
            break;
        case AST_ASSIGN:
            //TODO
            break;
        case AST_IF:
            //TODO
            break;
        case AST_WHILE:
            //TODO
            break;
        case AST_CALL:
            //TODO
            break;
        case AST_BINOP:
            //TODO
            break;
        case AST_VAR:
            //TODO
            break;
        case AST_LITERAL:
            //TODO
            break;
        case AST_BLOCK:
            //TODO
            break;
        default:
            break;
    }


}

void generace_kodu(ast_tree_t *tree){

    //vygeneruje hlavičku
    printf(".IFJcode26\n");
    
    //pruchod stromem
    if(tree != NULL){
    pruchod_stromem(tree->root);
    }
}