#include "codegen.h"


//funkce pro tisknuti literalu
//vyuzivaji funkce ast_literal(), case_print()
void print_literal(ast_node_t *node){
    switch (node->return_type){
        case INT: //datovy typ literalu je int
            printf("int@%d", node->data.literal.i_val);
            break;
        case DOUBLE: //datovy typ literalu float
            printf("float@%a", node->data.literal.d_val);
            break;
        case STRING: //datovy typ literal je string
            printf("string@"); //potreba poresit konvence zapisovani stringu podle pravidel IFJcode26
                for(int i = 0; node->data.literal.string_val[i] != '\0'; i++){ 
                    if(node->data.literal.string_val[i] == 32){
                        printf("\\032");
                    } else if (node->data.literal.string_val[i] == 92){
                        printf("\\092");
                    } else if (node->data.literal.string_val[i] == 35){
                        printf("\\035");
                    } else if (node->data.literal.string_val[i] <= 32){
                        printf("\\%03d",node->data.literal.string_val[i]); //pokud je hodnota znaku <= 32, kod vypise ciselnou hodnotu ve formatu \0xx
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

//funkce pro tisknuti promennych
//vyuzivaji funkce case_print(), ast_def_var(), ast_var(), ast_assign()
void print_var(ast_node_t *node){

    switch (node->data.variable.frame){
        case LOCAL:
            printf("LF@%s", node->data.variable.name);
            break;
        case GLOBAL:
            printf("GF@%s", node->data.variable.name);
            break;
        case TMP:
            printf("TF@%s", node->data.variable.name);
            break;
        default:
            break;
        }

}

//funkce pro specialni pripad kdy volanou funkci je print()
//vyuziva funkce ast_call
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
    printf("\n");
}

//funkce pro generovani literalu
void ast_literal(ast_node_t *node){
    printf("PUSHS ");
    print_literal(node);
    printf("\n");
}

//funkce pro generovani definice promenne
void ast_var_def(ast_node_t *node){

    printf("DEFVAR ");
    print_var(node);
    printf("\n");

}

//funkce pro generovani kodu za pripadu, ze je promenna pouzita (napr. jako parametr)
void ast_var(ast_node_t *node){

      printf("PUSHS ");
      print_var(node);
      printf("\n");

}

//funkce pro genrovani binarnich operaci
//TODO, potreba upravit ast, dokoncit zbytek operaci
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


//funkce pro pridani hodnoty do promenne
void ast_assign(ast_node_t *node){

    projdi_strom(node->data.assign.source); //vyhodnoceni zdroje, ktery muze byt typu AST_LITRAL, AST_VAR nebo AST_BINOP
    printf("POPS "); //Predani z vrcholu zasobniku
    print_var(node->data.assign.dest);
    printf("\n");

}


void ast_func_def(ast_node_t *node){



}

//funkce pro volani funkci
//TODO: NEDOKONCENE, potreba poresit logiku volani funkci + dalsi specialni volani funkci
void ast_call(ast_node_t *node){

    if (strcmp(node->data.func.name, "print") == 0){
            case_print(node);
    } else {
        printf("CALL ");//dokoncit, velmi nedokoncene!!! 
        printf("%s", node->data.func.name);
        printf("\n");
    }
    
}

/*

void ast_if(ast_node_t *node){



}

void ast_while(ast_node_t *node){



}


void ast_block(ast_node_t *node){



}
*/

        

void projdi_strom(ast_node_t *node){

    if(node == NULL){
        return;
    }
    while(node != NULL){
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
    node = node->next;
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