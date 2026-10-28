#include "parser.h"
#include "automat.h"
#include "symtable.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* TODODODO:
    cyklus na tokeny a switch na spousteni funkci
    jednotlive funkce
    spravne ocislovani chyb


*/

token_t current_token;


token_t get_token(){
    // Zavolani pro dalsi token

    token_t t;
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
        return 1; // CHYBA
    }

    next(); // druhy token musi byt ifj

    if (current_token.type != TOKEN_IDENTIFIER || strcmp(current_token.str_val, "ifj26") != 0){

        return 2; // CHYBA
    }
    
   next(); // treti token musi byt .
    
    if (current_token.type != TOKEN_DOT){
        return 3; // CHYBA
    }
    
    next(); // ctvrty token musi byt *

    if(current_token.type != TOKEN_MUL){
        return 4; // CHYBA
    }
    next();
    return 0;
}

int parse() {
    int result;
    int main = 0;
    tree_t *global_symtable = symtable_init();
    
    // nacteni tokenu
    next();

    result = parse_prologue();
    if (result != 0) {
        return 1; // CHYBA // Chyba v prologu
    }

    // Hlavní cyklus
    while (current_token.type != TOKEN_EOF) {
        switch (current_token.type) {
            case KEYWORD_DEF:
                // Začátek definice funkce
                result = parse_function(global_symtable);
                if (result != 0){
                    return 1; // CHYBA
                } 
                break;
                
            case TOKEN_EOL:
                
                next();
                break;
            
            case TOKEN_AT:
                next();
                if (current_token.type != TOKEN_IDENTIFIER || strcmp(current_token.str_val, "main") != 0){
                    return 2; // CHYBA
                }

                next();                
                
                while(current_token.type == TOKEN_EOL){
                    next();
                }

                if(current_token.type != KEYWORD_DEF){
                    return 1; // CHYBA 
                }

                if(main) {
                    return 1; // CHYBA vice mainu
                }
                main = 1;

                result = parse_main();
                if (result != 0) {
                    return 1; //CHYBA
                }

                break;

            default:
                // Neočekávaný token
                return 2; // CHYBA // Syntaktická chyba
        }
    }
    if(main != 1){
        return 1; // CHYBA 
    }

    symtable_destroy_tree(global_symtable);
    return 0; // 
}

int parse_function(tree_t *global_symtable) {

    next(); //z def na další token

    // název funkce (identifikátor)
    if (current_token.type != TOKEN_IDENTIFIER) {
        return 2; // CHYBA // Syntaktická chyba
    }

    tree_t *local_symtable = symtable_init();


    symtable_destroy_tree(local_symtable);
    return 0; 
}

int parse_main(){
    next(); //z def na další token

    // název funkce (identifikátor)
    if (current_token.type != TOKEN_IDENTIFIER) {
        return 2; // CHYBA // Syntaktická chyba
    }
    tree_t *local_symtable = symtable_init();

    




    symtable_destroy_tree(local_symtable);
    return 0; 
}
