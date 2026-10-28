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
    current_token = get_token();
}

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
    return 0;
}

int parse() {
    int result;
    int main = 0;
    tree_t *global_symtable = symtable_init(); // globalni tabulka pro funkce
    if (global_symtable == NULL) {
        return 99; // Interní chyba alokace
    }
    register_functions(global_symtable); // nacteni vestavenych funkci
    // nacteni tokenu
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
            
            case TOKEN_AT:
                next();
                if (current_token.type != TOKEN_IDENTIFIER || strcmp(current_token.str_val, "main") != 0){
                    symtable_destroy_tree(global_symtable); // uklid
                    return 2; // CHYBA
                }

                next();                
                
                while(current_token.type == TOKEN_EOL){
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
    return 0; // 
}

int parse_function(tree_t *global_symtable) {
    
    next(); //z def na další token

    // název funkce (identifikátor)
    if (current_token.type != TOKEN_IDENTIFIER) {
        return 2; // CHYBA // Syntaktická chyba
    }

    tree_t *local_symtable = symtable_init();
    if (local_symtable == NULL) {
        return 99; // Interní chyba alokace
    }

    
    
    if(symtable_lookup(global_symtable,current_token.str_val) == NULL){
        symtable_insert(global_symtable,current_token.str_val,TYPE_STRING,false,current_token.line);
    }else{
        symtable_destroy_tree(local_symtable);
        return  4;
    }



    symtable_destroy_tree(local_symtable);
    return 0; 
}

int parse_main(tree_t *global_symtable){
    next(); //z def na další token

    // název funkce (identifikátor)
    if (current_token.type != TOKEN_IDENTIFIER) {
        return 2; // CHYBA // Syntaktická chyba
    }
    tree_t *local_symtable = symtable_init();
    if (local_symtable == NULL) {
        return 99; // Interní chyba alokace
    }
    

        
    if(symtable_lookup(global_symtable,current_token.str_val) == NULL){
        symtable_insert(global_symtable,current_token.str_val,TYPE_STRING,false,current_token.line); /////PREDELAT
    }else{
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
    control |= symtable_insert(global_symtable,"strLenght",TYPE_INT, false, 0 );
    control |= symtable_insert(global_symtable,"subStr",TYPE_STRING, false, 0 );
    control |= symtable_insert(global_symtable,"ord",TYPE_INT, false, 0 );
    control |= symtable_insert(global_symtable,"chr",TYPE_STRING, false, 0 );

    if(control != 0 ){
        return 99;
    }
    return 0;
}