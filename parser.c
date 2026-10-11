#include "parser.h"
#include <stdlib.h>
#include <stdio.h>


/* TODODODO:
    cyklus na tokeny a switch na spousteni funkci
    jednotlive funkce
    spravne ocislovani chyb


*/


/* NOTES
    bylo by super aby tokeny meli ID at jdou hazet do switche jednoduse
*/

token token; // = current token

token get_token(){

    //zavola lexer pro dalsi token

}

int parse(){
    int result;

    result = parse_prologue();
    if(result != 0):
        return 1; // chyba neni spravny zacatek 


    while(token != "EOF"){
        token = get_token();

        switch(token.id)
            case 1: // token def
                parse_function()
                


    }



}


int parse_prologue(){

   // prvni token musi byt 
   // import  
   // dalsi jeden token? asi nejaky special
   // ifj26.*

    if (token == "import") :
        token = get_token();
        if (token == "ifj"):
            token = get_token();
            if(token == "."):
                token = get_token();
                if(token== "*"):
                    return 0;
                else:
                return 4;
            return 3;
        return 2;
    return 1;
}

int parse_function(){}

int parse_statement(){}

int parse_math(){}

void write_to_sym(){} // na zapisovani promennych do symtable

void send_to_code(){} // na posilani do generovani kodu