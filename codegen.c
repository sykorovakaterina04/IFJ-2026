#include "codegen.h"

//staticka promenna pro funkci ast_block, aby slo rozeznat zda se nachazi v tele funkce
static ast_node_t *curr_func = NULL;

//funkce pro tisknuti literalu
//vyuzivaji funkce ast_literal(), case_print()
void print_literal(ast_node_t *node){
    switch (node->data.literal.type){
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
                    } else if ((unsigned char)node->data.literal.string_val[i] < 32){
                        printf("\\%03d",(unsigned char)node->data.literal.string_val[i]); //pokud je hodnota znaku <= 32, kod vypise ciselnou hodnotu ve formatu \0xx
                    } else {
                        printf("%c",node->data.literal.string_val[i]);
                    }
                }
            break;
        case TYPE_UNIT:
                printf("string@()");
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

void definuj_defvar(ast_node_t *node){

    if(node == NULL){
        return;
    }


    if(node->type == AST_VAR_DEF){
        printf("DEFVAR ");
        print_var(node);
        printf("\n");
    } else if(node->type == AST_BLOCK){
        for(ast_node_t *line = node->data.block.first_line; line != NULL; line = line->next){
            definuj_defvar(line);
        }
    } else if (node->type == AST_IF) {
        definuj_defvar(node->data.condicionals.body);
        definuj_defvar(node->data.condicionals.else_body);
    } else if (node->type == AST_WHILE) {
        definuj_defvar(node->data.condicionals.body);
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
        printf("WRITE string@()\n");
    } else {

        projdi_strom(node->data.func.args);
        printf("POPS GF@$tmp1\n");
        printf("WRITE GF@$tmp1\n");
    }
    printf("PUSHS string@()\n");
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
    printf("STRLEN GF@$tmp1 GF@$tmp1\n");
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

void case_subStr(ast_node_t *node){
    
     if(node->data.func.args == NULL){
        return;
    }

    static int cnt = 0;
    int id = cnt++;

    projdi_strom(node->data.func.args); //nahazi argumenty na zasobnik

    printf("POPS GF@$tmp3\n"); //end
    printf("POPS GF@$tmp2\n"); //start
    printf("POPS GF@$tmp1\n"); //string

    //kontrola jestli index end neni negativni
    //                    end < 0 ?
    printf("LT GF@$tmp4 GF@$tmp3 int@0\n"); //GF@$tmp4 uklada bool hodnotu
    printf("JUMPIFEQ $wrong_input_substr_%d GF@$tmp4 bool@true\n",id);

    //kontrola jestli index start neni negativni
    //                    start < 0 ?
    printf("LT GF@$tmp4 GF@$tmp2 int@0\n"); //GF@$tmp4 uklada bool hodnotu
    printf("JUMPIFEQ $wrong_input_substr_%d GF@$tmp4 bool@true\n",id);

    //kontrola jestli start vetsi nez end
    //                    start > end ?
    printf("GT GF@$tmp4 GF@$tmp2 GF@$tmp3\n");
    printf("JUMPIFEQ $wrong_input_substr_%d GF@$tmp4 bool@true\n",id);

    //kontrola jestli delka retezce neni mensi nebo rovna indexu start
    printf("STRLEN GF@$tmp5 GF@$tmp1\n");
    //                    start > strlen(value)
    printf("GT GF@$tmp4 GF@$tmp2 GF@$tmp5\n");
    printf("JUMPIFEQ $wrong_input_substr_%d GF@$tmp4 bool@true\n",id);
    //                    start = strlen(value)
    printf("EQ GF@$tmp4 GF@$tmp2 GF@$tmp5\n");
    printf("JUMPIFEQ $wrong_input_substr_%d GF@$tmp4 bool@true\n",id);

    //kontrola jestli end neni vetsi nez delka retezce
    printf("STRLEN GF@$tmp5 GF@$tmp1\n");
    //                    start > strlen(value)
    printf("GT GF@$tmp4 GF@$tmp3 GF@$tmp5\n");
    printf("JUMPIFEQ $wrong_input_substr_%d GF@$tmp4 bool@true\n",id);

    printf("JUMP $all_ok_subStr_%d\n", id);// vsechno ok, neni potreba program ukoncit, tak skaceme na reseni funkce

    printf("LABEL $wrong_input_substr_%d\n", id);
    printf("EXIT int@40\n");

    printf("LABEL $all_ok_subStr_%d\n", id);

    //do GF@$tmp5 ulozime prazdny retezec, aby se k nemu postupne pomoci CONCAT mohlo pridavat
    //(celkovou delku retezce uz potreba nebude)
    printf("MOVE GF@$tmp5 string@\n");
    //navesti pro while-loop (start < end, start++)
    printf("LABEL $loop_substr_%d\n", id);
    //kontrola podminky
    //                    start < end ?
    printf("LT GF@$tmp4 GF@$tmp2 GF@$tmp3\n");
    printf("JUMPIFEQ $end_substr_%d GF@$tmp4 bool@false\n", id); //podminka neplati, cas skoncit

    //logika skladani noveho retezce
    printf("GETCHAR GF@$tmp4 GF@$tmp1 GF@$tmp2\n"); //vytahneme znak na aktualnim indexu
    printf("CONCAT GF@$tmp5 GF@$tmp5 GF@$tmp4\n"); //pridane znak do noveho retezce
    printf("ADD GF@$tmp2 GF@$tmp2 int@1\n"); //zvyseni hodnoty indexu start o 1
    printf("JUMP $loop_substr_%d\n", id); //zopakovani while-loopu, kontrola podminky...

    printf("LABEL $end_substr_%d\n", id);
    printf("PUSHS GF@$tmp5\n"); //posleme konecny podretezec na zasobnik

}

//konec implementace vestavenych funkci

//funkce pro generovani literalu
void ast_literal(ast_node_t *node){
    printf("PUSHS ");
    print_literal(node);
    printf("\n");

     //pripad kdy conversion_type neni shodny s type
      if(node->data.literal.conversion_type != node->data.literal.type){
        if((node->data.literal.type == TYPE_INT) && (node->data.literal.conversion_type == TYPE_DOUBLE)){
            printf("INT2FLOATS\n");
        }
      }
}

//funkce pro generovani definice promenne
void ast_var_def(ast_node_t *node){

    //samotna funkce nesmi nic tisknout
    //DEFVAR ted tiskne pouze ast_func_def pomoci funkce definuj_defvar
    //zamezi se tim nasobnemu tisknuti definice promennych

}

//funkce pro generovani kodu za pripadu, ze je promenna pouzita (napr. jako parametr)
void ast_var(ast_node_t *node){
    
      printf("PUSHS ");
      print_var(node);
      printf("\n");

      //pripad kdy conversion_type neni shodny s type
      if(node->data.variable.conversion_type != node->data.variable.type){
        if((node->data.variable.type == TYPE_INT) && (node->data.variable.conversion_type == TYPE_DOUBLE)){
            printf("INT2FLOATS\n");
        }
      }
}

//funkce pro genrovani binarnich operaci
//TODO potreba poresit pripady kdy potreba poresit konverze mezi datovymi typy
void ast_binop(ast_node_t *node){

    zpracuj_uzel(node->data.binop.left);
    //pokud argument neni var ani literal je stale potreba overit jestli jsou typy shodne
    if((node->data.binop.left->return_type == TYPE_INT) && (node->data.binop.right->return_type == TYPE_DOUBLE)){
        //pojistka proti double konverzi
        if((node->data.binop.left->type != AST_LITERAL) && (node->data.binop.left->type != AST_VAR)){
            printf("INT2FLOATS\n");
        }
    }
    zpracuj_uzel(node->data.binop.right);
     if((node->data.binop.left->return_type == TYPE_DOUBLE) && (node->data.binop.right->return_type == TYPE_INT)){
        if((node->data.binop.right->type != AST_LITERAL) && (node->data.binop.right->type != AST_VAR)){
            printf("INT2FLOATS\n");
        }
    }

    //kontroly pro konverze preskoci kdyz se node == TYPE_STRING, ten se bude resit specialnim pripadem CONCAT 

    switch (node->data.binop.type){
        case ADD:
        if(node->return_type == TYPE_STRING){
            //volani CONCAT neni zasobnikova funkce, potreba si projpucit dve promenne z GF@$tmpx
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
        case EQ:
            printf("EQS\n");
            break;
        case NEQ:
            printf("EQS\n");
            printf("NOTS\n");
            break;
        case LT:
            printf("LTS\n");
            break;
        case GT:
            printf("GTS\n");
            break;
        case LEQ:
            printf("GTS\n");
            printf("NOTS\n");
            break;
        case GEQ:
            printf("LTS\n");
            printf("NOTS\n");
            break;
        default:
            break;
        
    }
}

//funkce pro pridani hodnoty do promenne
void ast_assign(ast_node_t *node){

    zpracuj_uzel(node->data.assign.source); //vyhodnoceni zdroje, ktery muze byt typu AST_LITRAL, AST_VAR nebo AST_BINOP
    printf("POPS "); //Predani z vrcholu zasobniku
    print_var(node->data.assign.dest);
    printf("\n");

}

//funkce pro volani funkci
//TODO: NEDOKONCENE, potreba poresit logiku volani funkci
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
    }else if(strcmp(node->data.func.name, "subStr") == 0){
            case_subStr(node);
    }else {
        projdi_strom(node->data.func.args);

        printf("CALL ");
        printf("%s", node->data.func.name);
        printf("\n");
    }
    
}


void ast_if(ast_node_t *node){

    static int cnt = 0;
    int id = cnt++;

    if(node->data.condicionals.cond == NULL){
        return;
    } else if (node->data.condicionals.else_body == NULL){

        zpracuj_uzel(node->data.condicionals.cond);

        printf("PUSHS bool@false\n");
        printf("JUMPIFEQS $if_end_%d\n", id);

        zpracuj_uzel(node->data.condicionals.body);

        printf("LABEL $if_end_%d\n", id);
    } else {

        zpracuj_uzel(node->data.condicionals.cond);

        printf("PUSHS bool@false\n");
        printf("JUMPIFEQS $else_body_%d\n", id);

        zpracuj_uzel(node->data.condicionals.body);
        printf("JUMP $if_end_%d\n", id);

        printf("LABEL $else_body_%d\n", id);
        zpracuj_uzel(node->data.condicionals.else_body);

        printf("LABEL $if_end_%d\n", id);

    }

}

void ast_while(ast_node_t *node){

    static int cnt = 0;
    int id = cnt++;

    if(node->data.condicionals.cond == NULL){
        return;
    } else {

        printf("LABEL $while_top_%d\n",id);
        zpracuj_uzel(node->data.condicionals.cond);

        printf("PUSHS bool@false\n");
        printf("JUMPIFEQS $while_end_%d\n",id);

        zpracuj_uzel(node->data.condicionals.body);
        printf("JUMP $while_top_%d\n", id);

        printf("LABEL $while_end_%d\n", id);
    }

}


void ast_block(ast_node_t *node){

    //kontrola prazdneho uzlu
    if(node->data.block.first_line == NULL){
        return;
    }
    //kontrola zda se nachazime v tele funkce
    bool is_afunc_body = (curr_func != NULL && curr_func->data.func.body == node);

    ast_node_t *curr = node->data.block.first_line; //promenna pro zkraceni zapisu

    //prochazime po krocich kvuli cisteni zasobniku
    while(curr != NULL){

        //kontrola zda se nenachazime na konci funkce 
        bool is_last = (is_afunc_body && curr->next == NULL);

        zpracuj_uzel(curr);

        bool is_expr = (curr->type == AST_VAR ||
            curr->type == AST_LITERAL ||
            curr->type == AST_CALL||
            curr->type == AST_BINOP);

        if(is_last){
            if(curr_func->return_type == TYPE_UNIT){
                if(is_expr && curr->return_type != TYPE_UNIT){
                    printf("POPS GF@$tmp1\n");
                }
            printf("PUSHS string@()\n");
            } else {
                if (is_expr && curr->return_type == TYPE_INT && curr_func->return_type == TYPE_DOUBLE) {
                    printf("INT2FLOATS\n");
                }
            }
            
        } else {
            if (is_expr && curr->return_type != TYPE_UNIT) {
                printf("POPS GF@$tmp1\n");
        }


    curr = curr->next; //posun cyklu
    }
    }

}

//TODO: potreba zkontrolovat
void ast_return(ast_node_t *node){

    if(node->data.node_return.var != NULL){
        zpracuj_uzel(node->data.node_return.var);
    }

    if(node->data.node_return.func_def->return_type == TYPE_UNIT){
        if(node->data.node_return.var != NULL && node->data.node_return.var->return_type != TYPE_UNIT){
            printf("POPS GF@$tmp1\n");
        }
        printf("PUSHS string@()\n");
    } else {
        if(node->data.node_return.var != NULL &&
            node->data.node_return.var->return_type == TYPE_INT &&
            node->data.node_return.func_def->return_type == TYPE_DOUBLE
            ){
                printf("INT2FLOATS\n");
            }
    }
    printf("JUMP $end_func_%s\n", node->data.node_return.func_def->data.func.name);
}


void ast_func_def(ast_node_t *node){

    curr_func = node;

    printf("LABEL %s\n", node->data.func.name);

    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    int args_cnt = 0;
    for(ast_node_t *args = node->data.func.args; args != NULL; args = args->next){
        args_cnt++;
    }

    //pomocna promenna pro uchovani parametru funkce
    ast_node_t *params[args_cnt];
    int idx = 0;
    for(ast_node_t *arg = node->data.func.args; arg != NULL; arg = arg->next){
        params[idx] = arg;
        idx++;
    }

    //definice promennych a vytahnuti parametru ze zasobniku
    for (int i = args_cnt - 1; i >= 0; i--) {
            printf("DEFVAR LF@%s\n", params[i]->data.variable.name);
            printf("POPS LF@%s\n", params[i]->data.variable.name);
    }

    //projdi zbytek funkce a hned vypis lokalni promenne 
    definuj_defvar(node->data.func.body);

    zpracuj_uzel(node->data.func.body);

    printf("LABEL $end_func_%s\n", node->data.func.name);

    printf("POPFRAME\n");
    printf("RETURN\n");

    curr_func = NULL;
}

void zpracuj_uzel(ast_node_t *node){

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
                    ast_var(node);
                break;
            case AST_CALL:
                    ast_call(node);
                break;
            case AST_ASSIGN:
                    ast_assign(node);
                break;
            case AST_IF:
                    ast_if(node);
                break;
            case AST_WHILE:
                    ast_while(node);
                break;
            case AST_BLOCK:
                    ast_block(node);
                break;
            case AST_RETURN:
                    ast_return(node);
                break;
            case AST_FUNC_DEF:
                    ast_func_def(node);
                break;
            default:
                break;
    }

}

void projdi_strom(ast_node_t *node){

   while (node != NULL) {
        zpracuj_uzel(node);
        node = node->next;
    }
}

void generuj_kod(ast_tree_t *tree){

    //vygeneruje hlavičku
    printf(".IFJcode26\n");

    printf("DEFVAR GF@$tmp1\n");
    printf("DEFVAR GF@$tmp2\n");
    printf("DEFVAR GF@$tmp3\n");
    printf("DEFVAR GF@$tmp4\n");
    printf("DEFVAR GF@$tmp5\n");
    
    printf("CALL main\n");

    printf("EXIT int@0\n");

    //pruchod stromem
    if(tree != NULL){
    projdi_strom(tree->root);
    }
}