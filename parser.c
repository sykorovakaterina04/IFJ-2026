#include "parser.h"
#include "automat.h"
#include "symtable.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

token_t current_token;
static token_t ahead_token;
static bool looked_ahead = false;
token_t get_token(){
    // Zavolani pro dalsi token

    token_t t = automat(stdin);
    return t; // CHYBA

}

static void free_token(token_t *tok){
    if ((tok->type == TOKEN_IDENTIFIER || tok->type == TOKEN_STRING) && tok->str_val != NULL) {
        free(tok->str_val);
        tok->str_val = NULL;
    }
}

token_t peek_token(void){
    if(!looked_ahead){
        ahead_token = get_token(); 
        looked_ahead = true;
    }
    ahead_token;
}

void next(void){
    free_token(&current_token);
    if(looked_ahead){
        current_token = ahead_token;
        looked_ahead = false;
    }else{
        current_token = get_token();
    }
}

// hotovo zatim az propojeni s lexerem
int parse_prologue(){

   // prvni token musi byt 
   // import  
   // dalsi jeden token? asi nejaky special
   // ifj26.*
    while(current_token.type == TOKEN_EOL){
        next();
    }
    if(current_token.type != KEYWORD_IMPORT){
        return 2; // CHYBA
    }

    next(); // druhy token musi byt ifj

    if (current_token.type != TOKEN_IDENTIFIER || strcmp(current_token.str_val, "ifj26") != 0){

        return 2; // CHYBA
    }
    
   next(); // treti token musi byt .
    
    if (current_token.type != TOKEN_DOT){
        return 2; // CHYBA
    }
    
    next(); // ctvrty token musi byt *

    if(current_token.type != TOKEN_MUL){
        return 2; // CHYBA
    }
    next();
    if (current_token.type != TOKEN_EOL && current_token.type != TOKEN_EOF) {
        return 2; // CHYBA 
    }
    return 0;
}

int parse(void) {
    int result;
    int main = 0;
    tree_t *global_symtable = symtable_init(NULL); // globalni tabulka pro funkce
    if (global_symtable == NULL) {
        return 99; // Interní chyba alokace
    }
    if (register_functions(global_symtable) != 0) {
        symtable_destroy_tree(global_symtable);
        return 99; 
    }


    indent_stack_t indent_stack;
    if (indent_init(&indent_stack) != 0) {
        symtable_destroy_tree(global_symtable);
        return 99;
    }

    current_token.type = TOKEN_EOF;
    //nacteni tokoenu
    next();

    result = parse_prologue();
    if (result != 0) {
        indent_free(&indent_stack);
        free_token(&current_token);
        symtable_destroy_tree(global_symtable); // uklid
        return 2; // CHYBA // Chyba v prologu
    }

    while (current_token.type != TOKEN_EOF) {
        switch (current_token.type) {
            case KEYWORD_DEF:
        
                result = parse_function(global_symtable, &indent_stack);
                if (result != 0){
                    indent_free(&indent_stack);
                    free_token(&current_token);
                    symtable_destroy_tree(global_symtable); // uklid
                    return result; // CHYBA
                } 
                break;
                
            case TOKEN_EOL:
                
                next();
                break;
            
            case TOKEN_AT_MAIN:
                next();         
                while (current_token.type == TOKEN_EOL){ 
                    next(); 
                }
                if(current_token.type != KEYWORD_DEF){
                    indent_free(&indent_stack);
                    free_token(&current_token);
                    symtable_destroy_tree(global_symtable); // uklid
                    return 2; // CHYBA 
                }

                if(main) {
                    indent_free(&indent_stack);
                    free_token(&current_token);                   
                    symtable_destroy_tree(global_symtable); // uklid
                    return 4; // CHYBA vice mainu
                }
                main = 1;

                result = parse_main(global_symtable,&indent_stack);
                if (result != 0) {
                    indent_free(&indent_stack);
                    free_token(&current_token);
                    symtable_destroy_tree(global_symtable); // uklid
                    return result; //CHYBA
                }

                break;

            default:
                // Neočekávaný token
                indent_free(&indent_stack);
                free_token(&current_token);
                symtable_destroy_tree(global_symtable); // uklid
                return 2; // CHYBA // Syntaktická chyba
        }
    }



    if(main != 1){
        symtable_destroy_tree(global_symtable); // uklid
        free_token(&current_token);
        symtable_destroy_tree(global_symtable);
        return 3; // CHYBA 
    }
    indent_free(&indent_stack);
    symtable_destroy_tree(global_symtable); // uklid
    free_token(&current_token);
    return 0; // 
}

//nacitani datoveho typu
static int parse_type(data_type_t *out_type){
    switch (current_token.type)
    {
    case KEYWORD_INT:  // podle typu mezi zavorkama si to vratim zrovna
        *out_type = TYPE_INT;
        next();
        return 0;
    case KEYWORD_DOUBLE:
        *out_type = TYPE_DOUBLE;
        next();
        return 0;
    case KEYWORD_STRING:
        *out_type = TYPE_STRING;
        next();
        return 0;                   
    case KEYWORD_UNIT:
        *out_type = TYPE_UNIT;
        next();
        return 0;
    case KEYWORD_BOOLEAN:
        return 2; // ROZSIRENI AZ
    case TOKEN_IDENTIFIER:
        return 3; // neznamu typ

    default:
        return 2; // chyba
    }
}

// parametry mezi () pri vstupu jsem na prvnim tokenu za (
static int parse_params(tree_t *local_symtable, data_type_t **out_param_types, int *out_param_count){
    *out_param_types = NULL;
    *out_param_count = 0;

    while (current_token.type == TOKEN_EOL)
    {
        next();
    }
    
    //bez parametru
    if(current_token.type == TOKEN_RPAREN){
        return 0;
    }

    while(true){
        if(current_token.type != TOKEN_IDENTIFIER){
            free(*out_param_types);
            *out_param_types = NULL;
            return 2;
        }

        //duplicitni nazvy
        if(symtable_lookup(local_symtable, current_token.str_val) != NULL){
            free(*out_param_types);
            *out_param_types = NULL;
            return 4;
        }

        //kopie nazvu paramteru
        char *param_name = malloc(strlen(current_token.str_val)+1);
        if(param_name == NULL){
            free(*out_param_types);
            *out_param_types = NULL;
            return 99;
        }
        
        strcpy(param_name, current_token.str_val);
        int param_line = current_token.line;

        next(); // na :
        if(current_token.type != TOKEN_COLON){
            free(*out_param_types);
            free(param_name);
            *out_param_types = NULL;
            return 2;
        }

        next(); // typ parametru

        data_type_t param_type;

        int type_res = parse_type(&param_type);
        if(type_res != 0){
            free(*out_param_types);
            free(param_name);
            *out_param_types = NULL;
            return type_res; // v type res je ta chyba ktera se stala
        }

        int ins_res = symtable_insert(local_symtable, param_name,param_type,false,param_line);
        free(param_name);
        if(ins_res != 0){
            free(*out_param_types);
            *out_param_types = NULL;
            return 99;
        }

        data_type_t *tmp = realloc(*out_param_types, (*out_param_count + 1) * sizeof(data_type_t));
        if (tmp == NULL) {
            free(*out_param_types);
            *out_param_types = NULL;
            return 99;
        }
        *out_param_types = tmp;
        (*out_param_types)[*out_param_count] = param_type;
        (*out_param_count)++;



        // token , nebo )
        if(current_token.type == TOKEN_COMMA){
            next();
            bool eol = false;
            while(current_token.type == TOKEN_EOL){
                eol = true;
                next();
            }

            //carka muze byt jen po odradkovani
            if(current_token.type == TOKEN_RPAREN){
                if(!eol){
                    free(*out_param_types);
                    *out_param_types = NULL;
                    return 2;
                }
                break;
            }
        }else{         
            while(current_token.type == TOKEN_EOL){
                next();
            }
            if(current_token.type == TOKEN_RPAREN){
                break;
            }
            free(*out_param_types);
            *out_param_types = NULL;
            return 2;
        }
    }
    return 0;
}


int parse_block(tree_t *global_symtable, tree_t *local_symtable, indent_stack_t *indent_stack){


}

int parse_function(tree_t *global_symtable, indent_stack_t *indent_stack) {
    
    next(); //z def na nazev fce

    // název funkce (identifikátor)
    if (current_token.type != TOKEN_IDENTIFIER) {
        return 2; // CHYBA // Syntaktická chyba
    }

    //redefinice
    if (symtable_lookup(global_symtable, current_token.str_val) != NULL) {
        return 4;
    }


    //ulozeni nazvu
    char *func_name = malloc(strlen(current_token.str_val)+1);
    if(func_name == NULL){
        return 99;
    }

    strcpy(func_name, current_token.str_val);
    int func_line = current_token.line;

    next(); // na (

    if(current_token.type != TOKEN_LPAREN){
        free(func_name);
        return 2;
    }
    
    tree_t *local_symtable = symtable_init(global_symtable);
    if (local_symtable == NULL) {
        free(func_name);
        return 99; // Interní chyba alokace
    }
    data_type_t *param_types = NULL;
    int param_count = 0;

    next(); // do ()
    int res = parse_params(local_symtable,&param_types,&param_count);
    if (res != 0) {
        free(func_name);
        symtable_destroy_tree(local_symtable);
        return res;
    }
    next(); // posun za )

    //volitelny navratovy typ
    data_type_t return_type = TYPE_UNIT;
    if(current_token.type == TOKEN_COLON){
        next(); //za :
        res = parse_type(&return_type);
        if (res != 0) {
            free(func_name);
            free(param_types);
            symtable_destroy_tree(local_symtable);
            return res;
        }
    }

    //vkladani celkove funce do global symtable DODELAT CEKAM NA novou funkci v symtable
    int ins_res = 0; //toto prepsat funkci
    free(func_name);
    free(param_types);
    if(ins_res!=0){
        symtable_destroy_tree(local_symtable);
        return 99;
    }
    
    // musi byt =
    if(current_token.type != TOKEN_ASSIGN){
        symtable_destroy_tree(local_symtable);
        return 2;
    }

    next(); // za =

    //DODELAT SAMOTNE CTENI FUNKCE
    while(current_token.type != TOKEN_EOL && current_token.type != TOKEN_EOF){
        next();
    }

    symtable_destroy_tree(local_symtable);
    return 0; 
}

int parse_main(tree_t *global_symtable, indent_stack_t *indent_stack){
    next(); //z def na další token

    // název funkce (identifikátor)
    if (current_token.type != TOKEN_IDENTIFIER) {
        return 2; // CHYBA // Syntaktická chyba
    }

        
    if (symtable_lookup(global_symtable, current_token.str_val) == NULL) {
        symtable_insert(global_symtable, current_token.str_val, TYPE_UNIT, false, current_token.line);
    } else {
        return 4;
    }

    next();
    if(current_token.type != TOKEN_LPAREN){
    
        return 2;
    }
    next();
    while (current_token.type == TOKEN_EOL) {
        next();
    }

    if(current_token.type != TOKEN_RPAREN){
        if(current_token.type == TOKEN_IDENTIFIER){
            return 5;
        }
        return 2;
    }

    next();
    if (current_token.type == TOKEN_COLON) {
        next();
    if (current_token.type == KEYWORD_INT || current_token.type == KEYWORD_DOUBLE || current_token.type == KEYWORD_STRING) {  
          return 6; // @main musi mit navratovy typ  unit pokud je tam jiny je to chyba 6
        }
        if (current_token.type != KEYWORD_UNIT) {
            return 2; // pokud tam neni zadny typ je to chyba 2
        }
        next(); // posun za 'Unit'
    }

    if(current_token.type != TOKEN_ASSIGN){
        return 2;

    }
    next();

    tree_t *local_symtable = symtable_init(global_symtable);
    if (local_symtable == NULL) {
        return 99; // Interní chyba alokace
    }
    



    symtable_destroy_tree(local_symtable);
    return 0; 
}

int register_functions(tree_t *global_symtable){
    int control = 0;

    control |= symtable_insert(global_symtable,"print",FUNCTION,TYPE_UNIT, 0,1 );
    control |= symtable_insert(global_symtable,"readLine",FUNCTION,TYPE_STRING, 0,0 );
    control |= symtable_insert(global_symtable,"readInt",FUNCTION,TYPE_INT, 0,0 );
    control |= symtable_insert(global_symtable,"readDouble",FUNCTION,TYPE_DOUBLE, 0,0);
    control |= symtable_insert(global_symtable,"toStr",FUNCTION,TYPE_STRING, 0,1 );
    control |= symtable_insert(global_symtable,"toInt",FUNCTION,TYPE_INT, 0,1 );
    control |= symtable_insert(global_symtable,"strLength",FUNCTION,TYPE_INT, 0,1 );
    control |= symtable_insert(global_symtable,"subStr",FUNCTION,TYPE_STRING, 0,3 );
    control |= symtable_insert(global_symtable,"ord",FUNCTION,TYPE_INT, 0,2 );
    control |= symtable_insert(global_symtable,"chr",FUNCTION,TYPE_STRING, 0,1 );

    if(control != 0 ){
        return 99;
    }   
    return 0;
}