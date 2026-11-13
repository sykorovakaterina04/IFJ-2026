

#include "parser.h"
#include "automat.h"
#include "symtable.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

token_t current_token;

token_t get_token(){
    // Zavolani pro dalsi token

    token_t t = automat(stdin);
    return t; // CHYBA

}

void next(){
    free_token(&current_token);
    current_token = get_token();
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

int parse() {
    int result;
    int main = 0;
    tree_t *global_symtable = symtable_init(); // globalni tabulka pro funkce
    if (global_symtable == NULL) {
        return 99; // Interní chyba alokace
    }
    if (register_functions(global_symtable) != 0) {
        symtable_destroy_tree(global_symtable);
        return 99; 
    }
    //nacteni tokoenu
    next();

    result = parse_prologue();
    if (result != 0) {
        symtable_destroy_tree(global_symtable); // uklid
        return 2; // CHYBA // Chyba v prologu
    }

    // Hlavní cyklus
    while (current_token.type != TOKEN_EOF) {
        switch (current_token.type) {
            case KEYWORD_DEF:
                // Začátek definice funkce
                result = parse_function(global_symtable);
                if (result != 0){
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
                    symtable_destroy_tree(global_symtable); // uklid
                    return 2; // CHYBA 
                }

                if(main) {
                    symtable_destroy_tree(global_symtable); // uklid
                    return 4; // CHYBA vice mainu
                }
                main = 1;

                result = parse_main(global_symtable);
                if (result != 0) {
                    symtable_destroy_tree(global_symtable); // uklid
                    return result; //CHYBA
                }

                break;

            default:
                // Neočekávaný token
                symtable_destroy_tree(global_symtable); // uklid
                return 2; // CHYBA // Syntaktická chyba
        }
    }
    if(main != 1){
        symtable_destroy_tree(global_symtable); // uklid
        return 3; // CHYBA 
    }

    symtable_destroy_tree(global_symtable); // uklid
    free_token(&current_token);
    return 0; // 
}

static void free_token(token_t *tok){
    if ((tok->type == TOKEN_IDENTIFIER || tok->type == TOKEN_STRING) && tok->str_val != NULL) {
        free(tok->str_val);
        tok->str_val = NULL;
    }
}

//nacitani datoveho typu
static int parse_type(type_t *out_type){
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

int parse_main(tree_t *global_symtable){
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

    tree_t *local_symtable = symtable_init();
    if (local_symtable == NULL) {
        return 99; // Interní chyba alokace
    }
    

    symtable_destroy_tree(local_symtable);
    return 0; 
}

int parse_function(tree_t *global_symtable) {
    
    next(); //z def na další token

    // název funkce (identifikátor)
    if (current_token.type != TOKEN_IDENTIFIER) {
        return 2; // CHYBA // Syntaktická chyba
    }

    next();
    if(current_token.type != TOKEN_LPAREN){
    
        return 2;
    }

    next();

    
    tree_t *local_symtable = symtable_init();
    if (local_symtable == NULL) {
        return 99; // Interní chyba alokace
    }

    
    
    if(symtable_lookup(global_symtable,current_token.str_val) == NULL){
        if(symtable_insert(global_symtable,current_token.str_val,TYPE_STRING,false,current_token.line) !=0){
            symtable_destroy_tree(local_symtable);
            return 99;
        }
    }else{
        symtable_destroy_tree(local_symtable);
        return  4;
    }

    symtable_destroy_tree(local_symtable);
    return 0; 
}

int register_functions(tree_t *global_symtable){
    int control = 0;

    control |= symtable_insert(global_symtable,"print",TYPE_UNIT, false, 0 );
    control |= symtable_insert(global_symtable,"readLine",TYPE_STRING, false, 0 );
    control |= symtable_insert(global_symtable,"readInt",TYPE_INT, false, 0 );
    control |= symtable_insert(global_symtable,"readDouble",TYPE_DOUBLE, false, 0 );
    control |= symtable_insert(global_symtable,"toStr",TYPE_STRING, false, 0 );
    control |= symtable_insert(global_symtable,"toInt",TYPE_INT, false, 0 );
    control |= symtable_insert(global_symtable,"strLength",TYPE_INT, false, 0 );
    control |= symtable_insert(global_symtable,"subStr",TYPE_STRING, false, 0 );
    control |= symtable_insert(global_symtable,"ord",TYPE_INT, false, 0 );
    control |= symtable_insert(global_symtable,"chr",TYPE_STRING, false, 0 );

    if(control != 0 ){
        return 99;
    }   
    return 0;
}