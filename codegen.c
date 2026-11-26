/**
 * 
 * @file codegen.c
 * @author Katerina Sykorova (xsykork00)
 * @brief projekt IFJ a IAL 2026
 * @version 0.1
 * @date 2026-11-19
 * 
 */

#include "codegen.h"

//staticka promenna pro funkci ast_block, aby slo rozeznat zda se nachazi v tele funkce
static ast_node_t *curr_func = NULL;

/**
 * @brief pomocna funkce pro tisknuti literalu
 * 
 * @param node predany uzel
 */
void print_literal(ast_node_t *node){
    switch (node->data.literal.type){
        case TYPE_INT: //datovy typ literalu je int
            printf("int@%d", node->data.literal.i_val);
            break;
        case TYPE_DOUBLE: //datovy typ literalu float
            printf("float@%a", node->data.literal.d_val);
            break;
        case TYPE_STRING: //datovy typ literal je string
            printf("string@"); //konvence zapisovani stringu podle pravidel IFJcode26
                for(int i = 0; node->data.literal.string_val[i] != '\0'; i++){ 
                    if(node->data.literal.string_val[i] == 32){
                        printf("\\032");
                    } else if (node->data.literal.string_val[i] == 92){
                        printf("\\092");
                    } else if (node->data.literal.string_val[i] == 35){
                        printf("\\035");
                    } else if ((unsigned char)node->data.literal.string_val[i] < 32){
                        printf("\\%03d",(unsigned char)node->data.literal.string_val[i]); //pokud je hodnota znaku < 32, kod vypise ciselnou hodnotu ve formatu \xyz
                    } else {
                        printf("%c",node->data.literal.string_val[i]);
                    }
                }
            break;
        case TYPE_UNIT: //datovy typ literalu je unit
                printf("string@()");
            break;
        default:
            break; 
        //case TYPE_BOOL v pripade rozsireni BOOLTHEN
        }
}


/**
 * @brief pomocna funkce pro tisknuti promennych
 * 
 * @param node 
 */
void print_var(ast_node_t *node){

    //tisknuti promenne podle zadaneho ramce
    switch (node->data.variable.frame){
        case LOCAL:
            printf("LF@%s$%d", node->data.variable.name, node->data.variable.id);
            break;
        case GLOBAL:
            printf("GF@%s$%d", node->data.variable.name, node->data.variable.id);
            break;
        case TMP:
            printf("TF@%s$%d", node->data.variable.name, node->data.variable.id);
            break;
        default:
            break;
        }

}

/**
 * @brief pomocna funkce pro tisknuti definic promennych
 * 
 * @param node 
 */
void definuj_defvar(ast_node_t *node){

    //pokud je uzel prazdny, vrat se
    if(node == NULL){
        return;
    }

    //pokud nazazis na uzel ast_def_var zpracuj ho a vytiskni promennou
    if(node->type == AST_VAR_DEF){
       printf("DEFVAR LF@%s$%d\n", node->data.variable.name, node->data.variable.id);
        //rekurzivni prochazeni pri kazdem pripade bloku kodu - vytahne vsechny DEFVAR na zacatek funkce
    } else if(node->type == AST_BLOCK){
        for(ast_node_t *line = node->data.block.first_line; line != NULL; line = line->next){
            definuj_defvar(line);
        }
    } else if (node->type == AST_IF) {
        definuj_defvar(node->data.condicionals.body);
        if(node->data.condicionals.else_body != NULL){
        definuj_defvar(node->data.condicionals.else_body);
        }
    } else if (node->type == AST_WHILE) {
        definuj_defvar(node->data.condicionals.body);
    }

}

//nasledujic funkce implementuji specificke (vestavene) funkce pro AST_CALL

/**
 * @brief funkce pro vestavenou funkci print(value: Any): Unit
 * 
 * @param node 
 */
void case_print(ast_node_t *node){

    if(node->data.func.args == NULL){
        return;
    }

    //zadani: hodnota () typu Unit se vypíše jako řetězec "()"
    if(node->data.func.args->return_type == TYPE_UNIT){
        printf("WRITE string@()\n");
    } else {

        projdi_strom(node->data.func.args);
        printf("POPS GF@$tmp1\n");
        printf("WRITE GF@$tmp1\n"); //vsechno ostatni se vypisuje v souladu s instrukci WRITE
    }
    printf("PUSHS string@()\n"); //predani hodnot () pro jednotne cisteni zasobniku
}

/**
 * @brief funkce pro vestavenou funkci toStr(value: Any): String 
 * 
 * @param node 
 */
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

/**
 * @brief funkce pro vestavenou funkci toInt(value: Double): Int
 * 
 * @param node 
 */
void case_toInt(ast_node_t *node){

    if(node->data.func.args == NULL){
        return;
    }

    projdi_strom(node->data.func.args); //posle argumenty na zasobnik

    printf("FLOAT2INTS\n");
}

/**
 * @brief funkce pro vestavenou funkci strLength(value: String): Int
 * 
 * @param node 
 */
void case_strLength(ast_node_t *node){

     if(node->data.func.args == NULL){
        return;
    }

    projdi_strom(node->data.func.args); //posle argumenty na zasobnik

    printf("POPS GF@$tmp1\n");
    printf("STRLEN GF@$tmp1 GF@$tmp1\n"); 
    printf("PUSHS GF@$tmp1\n");

}

/**
 * @brief funkce pro vestavenou funkci readLine(): String
 * 
 */
void case_readLine(void){

    //vytvorime unikatni id pro cislovani jednotlivych navesti, tak aby se nam pri kazdem volani readLine
    //vytvorilo unikatni navesti
    static int cnt = 0;
    int id = cnt++;

    printf("READ GF@$tmp1 string\n");
    //kontrola jestli READ nevratilo nil
    printf("JUMPIFNEQ $skip_line_%d GF@$tmp1 nil@nil\n", id);
    printf("MOVE GF@$tmp1 string@\n"); //pokud ano, funkce vraci prazdny retezec
    printf("LABEL $skip_line_%d\n", id);

    printf("PUSHS GF@$tmp1\n");
}

/**
 * @brief funkce pro vestavenou funkci readInt(): Int 
 * 
 */
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

/**
 * @brief funkce pro vestavenou funkci readDouble(): Double
 * 
 */
void case_readDouble(void){

    //vytvorime unikatni id pro cislovani jednotlivych navesti, tak aby se nam pri kazdem volani readDouble
    //vytvorilo unikatni navesti
    static int cnt = 0;
    int id = cnt++;

    printf("READ GF@$tmp1 float\n");
    //pri EOF nebo chybnem vstupu READ vrati nil@nil, ale my potrebujeme aby vratil 0.0
    //pomuzeme si prikazem JUMPIFNEQ
    printf("JUMPIFNEQ $skip_double_%d GF@$tmp1 nil@nil\n", id);
    printf("MOVE GF@$tmp1 float@0x0p+0\n");
    printf("LABEL $skip_double_%d\n", id);
    printf("PUSHS GF@$tmp1\n");
}

/**
 * @brief funkce pro vestavenou funkci ord(value: String, index: Int): Int
 * 
 * @param node 
 */
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

    //zadani: "Je-li řetězec prázdný nebo index mimo rozsah, vrátí -1."

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

/**
 * @brief funkce pro vestavenou funkci chr(code: Int): String
 * 
 * @param node 
 */
void case_chr(ast_node_t *node){
    if(node->data.func.args == NULL){
        return;
    }

    projdi_strom(node->data.func.args); 

    printf("INT2CHARS\n");
}

/**
 * @brief funkce pro vestavenou funkci subStr(value: String, start: Int, end: Int): String
 * 
 * @param node 
 */
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
    //                    end > strlen(value)
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

/**
 * @brief funkce pro generovani literalu
 * 
 * @param node 
 */
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

/**
 * @brief funkce pro generovani definice promenne
 * 
 * @param node 
 */
void ast_var_def(void){

    //samotna funkce nesmi nic tisknout
    //DEFVAR ted tiskne pouze ast_func_def pomoci funkce definuj_defvar
    //zamezi se tim nasobnemu tisknuti definice promennych
    //DEFVAR se musi vytahnout na zacatek funkce, aby kazdy cyklus netvoril novy indenticky DEFVAR
}

/**
 * @brief funkce pro generovani kodu za pripadu, ze je promenna pouzita (napr. jako parametr)
 * 
 * @param node 
 */
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

/**
 * @brief funkce pro generovani binarnich operaci
 * 
 * @param node 
 */
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
    switch (node->data.binop.type){
        case ADD:
        if(node->return_type == TYPE_STRING){ //pokud mame pripad konkatence retezcu je potreba to poresit zvlast
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
            printf("NOTS\n"); //zadani: Pro vypocet neostrych nerovnosti lze pouzit AND/OR/NOT.
            break;
        case GEQ:
            printf("LTS\n");
            printf("NOTS\n");
            break;
        default:
            break;
        
    }
}

/**
 * @brief funkce pro pridani hodnoty do promenne
 * 
 * @param node 
 */
void ast_assign(ast_node_t *node){

    zpracuj_uzel(node->data.assign.source); //vyhodnoceni zdroje, ktery muze byt typu AST_LITERAL, AST_VAR, AST_BINOP nebo AST_CALL

      if(node->data.assign.source->type != AST_LITERAL && // AST_LITERAL a AST_VAR si konverze resi samy
        node->data.assign.source->type != AST_VAR &&
        node->data.assign.source->return_type == TYPE_INT &&
        node->data.assign.dest->data.variable.type == TYPE_DOUBLE){
            printf("INT2FLOATS\n");
    }

    printf("POPS "); //Predani z vrcholu zasobniku
    print_var(node->data.assign.dest);
    printf("\n");

}

/**
 * @brief funkce pro volani funkci
 * 
 * @param node 
 */
void ast_call(ast_node_t *node){

    //volani vestavenych funkci
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
        //PUSHS argumentu pred volanim funkce
        projdi_strom(node->data.func.args);

        printf("CALL ");
        printf("%s", node->data.func.name);
        printf("\n");
    }
    
}

/**
 * @brief funkce pro genrovani podminenych vyrazu
 * 
 * @param node 
 */
void ast_if(ast_node_t *node){

    //staticke pocitadlo pro unikatni id navesti (stejny princip jako u vestavenych funkci)
    static int cnt = 0;
    int id = cnt++;

    if(node->data.condicionals.cond == NULL){
        return;
    } else if (node->data.condicionals.else_body == NULL){

        zpracuj_uzel(node->data.condicionals.cond);

        printf("PUSHS bool@false\n"); //overeni jestli je podminka pravdiva
        printf("JUMPIFEQS $if_end_%d\n", id);

        zpracuj_uzel(node->data.condicionals.body);

        printf("LABEL $if_end_%d\n", id); //pokud neni, nemame else-body, if konci
    } else {

        zpracuj_uzel(node->data.condicionals.cond);

        printf("PUSHS bool@false\n"); // overeni jestli je podminka pravdiva
        printf("JUMPIFEQS $else_body_%d\n", id);

        zpracuj_uzel(node->data.condicionals.body); //pokud je, provedeme if body
        printf("JUMP $if_end_%d\n", id);

        printf("LABEL $else_body_%d\n", id); //pokud neni provedeme else-body
        zpracuj_uzel(node->data.condicionals.else_body);

        printf("LABEL $if_end_%d\n", id);

    }

}

/**
 * @brief funkce pro generovani while cyklu
 * 
 * @param node 
 */
void ast_while(ast_node_t *node){

    //staticke pocitadlo pro unikatni navesti
    static int cnt = 0;
    int id = cnt++;

    if(node->data.condicionals.cond == NULL){
        return;
    } else {

        printf("LABEL $while_top_%d\n",id);
        zpracuj_uzel(node->data.condicionals.cond);

        printf("PUSHS bool@false\n"); // pokud podminka neplati, ukonci while cyklus
        printf("JUMPIFEQS $while_end_%d\n",id);

        zpracuj_uzel(node->data.condicionals.body);
        printf("JUMP $while_top_%d\n", id); // pokud podminka plati, vykonec while-body a skoc na vrchol while-cyklu

        printf("LABEL $while_end_%d\n", id);
    }

}

/**
 * @brief funkce pro zpracovani blokovych useku kodu
 * 
 * @param node 
 */
void ast_block(ast_node_t *node){

    //kontrola prazdneho uzlu
    if(node->data.block.first_line == NULL){
        printf("PUSHS string@()\n");
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

            //kontrola jestsli jsme nenarazili na vyraz
            bool is_expr = (curr->type == AST_VAR ||
                curr->type == AST_LITERAL ||
                curr->type == AST_CALL||
                curr->type == AST_BINOP);

                //pokud jde o posledni prikaz
            if(is_last){
                if((curr_func->return_type == TYPE_UNIT) ||
                    (!is_expr && curr->type != AST_RETURN)){
                    if(is_expr){ //funkce s navratovym typem unit
                        printf("POPS GF@$tmp1\n");
                    }
                printf("PUSHS string@()\n");
                } else {
                    //konverze 
                    if ((curr->type == AST_CALL || curr->type == AST_BINOP) && 
                        curr->return_type == TYPE_INT && 
                        curr_func->return_type == TYPE_DOUBLE) {
                        printf("INT2FLOATS\n");
                    }
                }
            //nejsme na konci
            } else {
                if (is_expr) {
                    printf("POPS GF@$tmp1\n"); //prubezne cistime zasobnik
            }


    
        }
    curr = curr->next; //posun cyklu
    }

}

/**
 * @brief funkce pro generovani ukonceni funkce
 * 
 * @param node 
 */
void ast_return(ast_node_t *node){

    //zpracuj navratovou hodnotu return
    if(node->data.node_return.var != NULL){
        zpracuj_uzel(node->data.node_return.var);
    }

    //kontrola navratove hodnoty funkce a hodnoty co vraci return
    if(node->data.node_return.func_def->return_type == TYPE_UNIT){
        if(node->data.node_return.var != NULL){
            printf("POPS GF@$tmp1\n");
        }
        printf("PUSHS string@()\n");
    } else {
        if(node->data.node_return.var != NULL &&
            node->data.node_return.var->type != AST_LITERAL &&
            node->data.node_return.var->type != AST_VAR &&
            node->data.node_return.var->return_type == TYPE_INT &&
            node->data.node_return.func_def->return_type == TYPE_DOUBLE
            ){
                printf("INT2FLOATS\n");
            }
    }
    //vygeneruj skok na navesti pro ukonceni funkce
    printf("JUMP $end_func_%s\n", node->data.node_return.func_def->data.func.name);
}

/**
 * @brief funkce pro genrovani definice a tela funkce
 * 
 * @param node 
 */
void ast_func_def(ast_node_t *node){

    //momentalni funkce pro signalizaci ast_block
    curr_func = node;

    printf("LABEL %s\n", node->data.func.name);

    //vytvor ramec
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    //pocet argumentu
    int args_cnt = 0;
    for(ast_node_t *args = node->data.func.args; args != NULL; args = args->next){
        args_cnt++;
    }

    //pomocna promenna pro uchovani parametru funkce
    if (args_cnt > 0){
    ast_node_t *params[args_cnt];
    int idx = 0;
    for(ast_node_t *arg = node->data.func.args; arg != NULL; arg = arg->next){
        params[idx] = arg;
        idx++;
    }

    //definice promennych a vytahnuti parametru ze zasobniku
    //parametry se vytahuji v opacnem poradi nez byly na zasobnik dany
    for (int i = args_cnt - 1; i >= 0; i--) {
            printf("DEFVAR LF@%s$%d\n", params[i]->data.variable.name, params[i]->data.variable.id);
            printf("POPS LF@%s$%d\n", params[i]->data.variable.name, params[i]->data.variable.id);
    }
    }
    //projdi zbytek funkce a hned vypis definice promennych
    definuj_defvar(node->data.func.body);

    //zpracuj telo funkce
    zpracuj_uzel(node->data.func.body);

    printf("LABEL $end_func_%s\n", node->data.func.name);

    //ukonceni ramce
    printf("POPFRAME\n");
    printf("RETURN\n");

    //prirazeni NULL do curr_func pro spravne fungovani ast_block
    curr_func = NULL;
}

/**
 * @brief funkce pro zpracovani uzlu
 * 
 * @param node 
 */
void zpracuj_uzel(ast_node_t *node){

        switch (node->type){
            case AST_LITERAL: 
                    ast_literal(node);
                break;
            case AST_BINOP:
                    ast_binop(node);
                break;
            case AST_VAR_DEF:
                    ast_var_def();
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

/**
 * @brief funkce pro pruchod retezce uzlu
 * 
 * @param node 
 */
void projdi_strom(ast_node_t *node){

   while (node != NULL) {
        zpracuj_uzel(node);
        node = node->next;
    }
}

/**
 * @brief hlavni funkce geerujici kod
 * 
 * @param tree 
 */
void generuj_kod(ast_tree_t *tree){

    //vygeneruje hlavičku
    printf(".IFJcode26\n");

    //definice docasnych promennych
    printf("DEFVAR GF@$tmp1\n");
    printf("DEFVAR GF@$tmp2\n");
    printf("DEFVAR GF@$tmp3\n");
    printf("DEFVAR GF@$tmp4\n");
    printf("DEFVAR GF@$tmp5\n");
    
    //zavolej hlavni funkci
    printf("CALL %s\n", main_func_name);

    printf("EXIT int@0\n");

    //pruchod stromem
    if(tree != NULL){
    projdi_strom(tree->root);
    }
}