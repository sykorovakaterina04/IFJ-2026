#include "codegen.h"
#include <stdio.h>

void ast_literal(ast_node_t *node){
    switch (node->return_type){
        case INT:
            printf("PUSHS int@%d\n", node->data.literal.i_val);
            break;
        case DOUBLE:
            printf("PUSHS float@%a\n", node->data.literal.d_val);
            break;
        case STRING:
            printf("PUSHS string@"); //TODO: poresit uprava prebraneho stringu pro pravidla IFJcode26
                for(int i = 0; i < strlen(node->data.literal.string_val); i++){
                    if(node->data.literal.string_val[i] == 32){
                        printf("\\032");
                    } else if (node->data.literal.string_val[i] == 92){
                        printf("\\092");
                    } else if (node->data.literal.string_val[i] == 35){
                        printf("\\035");
                    } else if (node->data.literal.string_val[i] <= 32){
                        printf("\\%03d",node->data.literal.string_val[i]);
                    } else {
                        printf("%c",node->data.literal.string_val[i]);
                    }
                }
            printf("\n");
            break;
        //case TYPE_BOOL v pripade rozsireni BOOLTHEN
        }
}

void ast_var_def(ast_node_t *node){

    switch (node->data.variable.frame){
        case LOCAL:
            printf("DEFVAR LF@%s\n", node->data.variable.name);
            break;
        case GLOBAL:
            printf("DEFVAR GF@%s\n", node->data.variable.name);
            break;
        case TMP:
            printf("DEFVAR TF@%s\n", node->data.variable.name);
            break;
        }

}

void ast_binop(ast_node_t *node){

    projdi_strom(node->data.binop.left);
    projdi_strom(node->data.binop.right);

    switch (node->data.binop.op){
        case ('+'):
            printf("ADDS\n");
            break;
        case ('-'):
            printf("SUBS\n");
            break;
        case('*'):
            printf("MULS\n");
            break;
        case('/'): //je potreba poresit zda je vysledek operace int nebo double
            if(node->return_type == INT){
                printf("IDIVS\n");
            } else if (node->return_type == DOUBLE){
                printf("DIVS\n");
            }
            break;
        
    }
}

//promnnena je pouzita (napr jako parametr)
void ast_var(ast_node_t *node){

}

//do promenne se prirazuje hodnota
void ast_assing(ast_node_t *node){

    

}

void ast_func_def(ast_node_t *node){


}

void ast_if(ast_node_t *node){

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
        case AST_VAR_DEF:
                ast_var_def(node);
            break;
        case AST_VAR:
                ast_var(node);//WIP1
            break;
        case AST_FUNC_DEF:
            //WIP3
            break;
        case AST_ASSIGN:
            //WIP2
            break;
        case AST_IF:
            //WIP4
            break;
        case AST_WHILE:
            //TODO
            break;
        case AST_CALL:
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