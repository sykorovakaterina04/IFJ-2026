#include "codegen.h"
#include <stdio.h>


void print_literal(ast_node_t *node){
    switch (node->return_type){
        case INT:
            printf("int@%d", node->data.literal.i_val);
            break;
        case DOUBLE:
            printf("float@%a", node->data.literal.d_val);
            break;
        case STRING:
            printf("string@"); //TODO: poresit uprava prebraneho stringu pro pravidla IFJcode26
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
            break;
        default:
            break; 
        //case TYPE_BOOL v pripade rozsireni BOOLTHEN
        }
}

void print_var(ast_node_t *node){

    switch (node->data.variable.frame){
        case LOCAL:
            printf("LF@%s\n", node->data.variable.name);
            break;
        case GLOBAL:
            printf("GF@%s\n", node->data.variable.name);
            break;
        case TMP:
            printf("TF@%s\n", node->data.variable.name);
            break;
        default:
            break;
        }

}

void case_print(ast_node_t *node){

    printf("WRITE ");
        switch (node->data.func.args->type){
            case AST_LITERAL:
                print_literal(node->data.func.args);
                break;
            case AST_VAR:
                print_var(node->data.func.args);
                break;
            default:
                break;

    }
}

void ast_literal(ast_node_t *node){
    printf("PUSHS ");
    print_literal(node);
    printf("\n");
}

void ast_var_def(ast_node_t *node){

    printf("DEFVAR ");
    print_var(node);
    printf("\n");

}

//promnnena je pouzita (napr jako parametr)
void ast_var(ast_node_t *node){

      printf("PUSHS ");
      print_var(node);
      printf("\n");

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
        default:
            break;
        
    }
}


//do promenne se prirazuje hodnota
void ast_assign(ast_node_t *node){

    printf("MOVE ");
    print_var(node->data.assign.dest);
    switch (node->data.assign.source->type){
        case AST_VAR:
            print_var(node->data.assign.source);
            break;
        case AST_LITERAL:
            print_literal(node->data.assign.source);
            break;
        default:
            break;
    }
    printf("\n");

}

/*
void ast_func_def(ast_node_t *node){



}

void ast_if(ast_node_t *node){



}
*/

void ast_call(ast_node_t *node){

    if(node->data.func.name == "print"){
            case_print(node);
        }
        
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
        case AST_CALL:
                ast_call(node);
            break;
        case AST_ASSIGN:
                ast_assign(node);
            break;
        case AST_FUNC_DEF:
            //WIP3
            break;
        case AST_IF:
            //WIP4
            break;
        case AST_WHILE:
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