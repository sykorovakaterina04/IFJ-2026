#include "codegen.h"
#include <stdio.h>

void ast_literal(ast_node_t *node){
    switch (node->return_type){
        case TYPE_INT:
            printf("int@%d", node->literal.i_val);
            break;
        case TYPE_DOUBLE:
            printf("float@%a", node->literal.d_val);
            break;
        case TYPE_STRING:
            printf("string@"); //TODO: poresit uprava prebraneho stringu pro pravidla IFJcode26
                for(int i = 0; i < strlen(node->literal.string); i++){
                    if(node->literal.string[i] == 32){
                        printf("\\032");
                    } else if (node->literal.string[i] == 92){
                        printf("\\092");
                    } else if (node->literal.string[i] == 35){
                        printf("\\035");
                    } else if (node->literal.string[i] <= 32){
                        printf("\\%03d",node->literal.string[i]);
                    } else {
                        printf("%c",node->literal.string[i]);
                    }
                }
            break;
        //case TYPE_BOOL v pripade rozsireni BOOLTHEN
        }
}

void ast_binop(ast_node_t *node){
    
}

void projdi_strom(ast_node_t *node){

    if(node == NULL){
        return;
    }
    switch (node->type){
        case AST_LITERAL: 
                ast_literal(node);
            break;
        case AST_BINOP:
                ast_binop(node);
            break;
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
        case AST_VAR:
            //TODO
            break;
        case AST_BLOCK:
            //TODO
            break;
        default:
            break;
    }


}

void generuj_kodu(ast_tree_t *tree){

    //vygeneruje hlavičku
    printf(".IFJcode26\n");
    
    //pruchod stromem
    if(tree != NULL){
    projdi_strom(tree->root);
    }
}