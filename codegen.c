#include "codegen.h"

//funkce pro tisknuti literalu
//vyuzivaji funkce ast_literal(), case_print()
void print_literal(ast_node_t *node){
    switch (node->return_type){
        case TYPE_INT: //datovy typ literalu je int
            printf("int@%d", node->data.literal.i_val);
            break;
        case TYPE_DOUBLE: //datovy typ literalu float
            printf("float@%a", node->data.literal.d_val);
            break;
        case TYPE_STRING: //datovy typ literal je string
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

//nasledujic funkce implementuji specificke (vestavene) funkce pro AST_CALL

//funkce pro specialni pripad kdy volanou funkci je print()
//vyuziva funkce ast_call
void case_print(ast_node_t *node){

    if(node->data.func.args == NULL){
        return;
    }

    if(node->data.func.args->return_type == TYPE_UNIT){
        printf("WRITE string@()");
    } else {
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
    printf("\n");
}

void case_toStr(ast_node_t *node){

    if(node->data.func.args == NULL){
        return;
    }

    //opatreni pro case args == unit, v tom pripade se bude tisknout PUSHS string@()
    if(node->data.func.args->return_type == TYPE_INT || 
        node->data.func.args->return_type == TYPE_DOUBLE || 
        node->data.func.args->return_type== TYPE_STRING){

    projdi_strom(node->data.func.args); //posle argument na zasobnik

    if(node->data.func.args->return_type == TYPE_INT){
        printf("INT2STRS\n");
    }
    if(node->data.func.args->return_type == TYPE_DOUBLE){
        printf("FLOAT2STRS\n");
    }
    //pokud byl args == STRING, hodnota jiz byla dana na zasobnik a neni potreba ji konvertovat
    }

    if(node->data.func.args->return_type == TYPE_UNIT){
        printf("PUSHS string@()\n");
    }
}

void case_toInt(ast_node_t *node){

    if(node->data.func.args == NULL){
        return;
    }

    projdi_strom(node->data.func.args); //posle argumenty na zasobnik

    printf("FLOAT2INTS\n");
}

void case_strLength(ast_node_t *node){

     if(node->data.func.args == NULL){
        return;
    }

    projdi_strom(node->data.func.args);

    printf("POPS GF@$tmp1\n");
    printf("STRLEN GF@$tmp1 GF@_tmp1\n");
    printf("PUSHS GF@$tmp1\n");

}

void case_readLine(void){

    printf("READ GF@$tmp1 string\n");
    printf("PUSHS GF@$tmp1\n");
}

void case_readInt(void){

    //vytvorime unikatni id pro cislovani jednotlivych navesti, tak aby se nam pri kazdem volani readInt
    //vytvorilo unikatni navesti
    static int cnt = 0;
    int id = cnt++;

    printf("READ GF@$tmp1 int\n");
    //pri EOF nebo chybnem vstupu READ vrati nil@nil, ale my potrebujeme aby vratil 0
    //pomuzeme si prikazem JUMPIFNEQ
    printf("JUMPIFNEQ $skip_int_%d GF@$tmp1 nil@nil\n", id);
    printf("MOVE GF@$tmp1 int@0\n");
    printf("LABEL $skip_int_%d\n", id);
    printf("PUSHS GF@$tmp1\n");
}

void case_readDouble(void){

    //vytvorime unikatni id pro cislovani jednotlivych navesti, tak aby se nam pri kazdem volani readDouble
    //vytvorilo unikatni navesti
    static int cnt = 0;
    int id = cnt++;

    printf("READ GF@$tmp1 float\n");
    //pri EOF nebo chybnem vstupu READ vrati nil@nil, ale my potrebujeme aby vratil 0
    //pomuzeme si prikazem JUMPIFNEQ
    printf("JUMPIFNEQ $skip_double_%d GF@$tmp1 nil@nil\n", id);
    printf("MOVE GF@$tmp1 float@0x0p+0\n");
    printf("LABEL $skip_double_%d\n", id);
    printf("PUSHS GF@$tmp1\n");
}

void case_ord(ast_node_t *node){

    if(node->data.func.args == NULL){
        return;
    }

    static int cnt = 0;
    int id = cnt++;

    projdi_strom(node->data.func.args); //nahazi argumenty na zasobnik


    //potreba udelat operace mimo zasobnik - argumenty 'vytahneme vem'
    printf("POPS GF@$tmp2\n");//index
    printf("POPS GF@$tmp1\n");//string

    //"Je-li řetězec prázdný nebo index mimo rozsah, vrátí -1." -zadani

    //pro index je negativni
    //                   index <  0 ?
    printf("LT GF@$tmp3 GF@$tmp2 int@0\n"); //je index mensi jak 0?
    printf("JUMPIFEQ $wrong_input_ord_%d GF@$tmp3 bool@true\n", id); //LT ulozi bool hodnotu do GF@$tmp3

    //pro index je vetsi nez delka reteze/ retezec je pradny (jeho delka je 0)
    printf("STRLEN GF@$tmp4 GF@$tmp1\n"); //do GF@$tmp4 se ulozi delka retezce
    printf("JUMPIFEQ $wrong_input_ord_%d GF@$tmp4 int@0\n", id); //pokud je delka 0, retezec je prazdny
    //                  index < delka ?
    printf("LT GF@$tmp3 GF@$tmp2 GF@$tmp4\n");//je index mensi nez delka retezce?
    printf("JUMPIFEQ $wrong_input_ord_%d GF@$tmp3 bool@false\n", id); //pokud ne, skoc na chybu

    printf("JUMP $all_ok_ord_%d\n", id);//pokud se beh nezastavil na zadnem erroru, preskocime prirazovani -1

    printf("LABEL $wrong_input_ord_%d\n", id);
    printf("MOVE GF@$tmp5 int@-1\n"); //pokud bylo aktivovano jedno z chybovych hlaseni, dej na zasobnik -1
    printf("JUMP $ord_end_%d\n", id); //uz neni co resit, skoc na konec

    printf("LABEL $all_ok_ord_%d\n", id);
    //logika prevodu zanku na ascii kod
    printf("STRI2INT GF@$tmp5 GF@$tmp1 GF@$tmp2\n"); //do GF@$tmp5 se ulozi ascii kod znaku
    printf("LABEL $ord_end_%d\n", id);
    printf("PUSHS GF@$tmp5\n");

}

void case_chr(ast_node_t *node){
    if(node->data.func.args == NULL){
        return;
    }

    projdi_strom(node->data.func.args); 

    printf("INT2CHARS\n");
}

//konec implementace vestavenych funkci

//funkce pro generovani literalu
void ast_literal(ast_node_t *node){
    printf("PUSHS ");
    print_literal(node);
    printf("\n");

     //pripad kdy conversion_type neni shodny s type
      if(node->data.literal.conversion_type != node->return_type){
        if((node->return_type == INT) && (node->data.literal.conversion_type == DOUBLE)){
            printf("INT2FLOATS\n");
        }
      }
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

      //pripad kdy conversion_type neni shodny s type
      if(node->data.variable.conversion_type != node->data.variable.type){
        if((node->data.variable.type == INT) && (node->data.variable.conversion_type == DOUBLE)){
            printf("INT2FLOATS\n");
        }
      }
}

//funkce pro genrovani binarnich operaci
//TODO, potreba upravit ast, dokoncit zbytek operaci, potreba poresit pripady kdy potreba poresit konverze mezi datovymi typy
void ast_binop(ast_node_t *node){

    projdi_strom(node->data.binop.left);
    projdi_strom(node->data.binop.right);

    switch (node->data.binop.type){
        case ADD:
        if(node->return_type == TYPE_STRING){
            //volani CONVCAT neni zasobnikova funkce, potreba si projpucit dve z GF@_tmpx
            printf("POPS GF@$tmp2\n"); //prava strana
            printf("POPS GF@$tmp1\n");  //leva strana
            printf("CONCAT GF@$tmp1 GF@$tmp1 GF@$tmp2\n");
            printf("PUSHS GF@$tmp1\n");
        } else {
            printf("ADDS\n");
        }
            break;
        case SUB:
            printf("SUBS\n");
            break;
        case MUL:
            printf("MULS\n");
            break;
        case IDIV: //je potreba poresit zda je vysledek operace int nebo double
            printf("IDIVS\n");
            break;
        case DIV:
            printf("DIVS\n");
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

//pocet volani parametru je soucasti symtable
//lokalni promenne reseni semantika ?
//telo funkce TODO
void ast_func_def(ast_node_t *node){



}

//funkce pro volani funkci
//TODO: NEDOKONCENE, potreba poresit logiku volani funkci + dalsi specialni volani funkci
void ast_call(ast_node_t *node){

    if (strcmp(node->data.func.name, "print") == 0){
            case_print(node);
    }else if(strcmp(node->data.func.name, "toStr") == 0) {
            case_toStr(node);
    }else if(strcmp(node->data.func.name, "toInt") == 0){
            case_toInt(node);
    }else if(strcmp(node->data.func.name, "strLength") == 0){
            case_strLength(node);
    }else if(strcmp(node->data.func.name, "readLine") == 0){
            case_readLine();
    }else if(strcmp(node->data.func.name, "readInt") == 0){
            case_readInt();
    }else if(strcmp(node->data.func.name, "readDouble") == 0){
            case_readDouble();
    }else if(strcmp(node->data.func.name, "ord") == 0){
            case_ord(node);
    }else if(strcmp(node->data.func.name, "chr") == 0){
            case_chr(node);
    }else {
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

    printf("DEFVAR GF@$tmp1\n");
    printf("DEFVAR GF@$tmp2\n");
    printf("DEFVAR GF@$tmp3\n");
    printf("DEFVAR GF@$tmp4\n");
    printf("DEFVAR GF@$tmp5\n");

    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    
    //CALL main?

    //pruchod stromem
    if(tree != NULL){
    projdi_strom(tree->root);
    }
}