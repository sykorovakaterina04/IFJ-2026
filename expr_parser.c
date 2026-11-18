#include "expr_parser.h"
#include "parser.h"
#include <stdlib.h>
#include <string.h>

//symboly precedenci synt analyzy

enum {
    P_UN = 0,   // unarni + -  //-a 
    P_MUL,      // * /
    P_ADD,      // binarni + - // a - b
    P_REL,      // < <= > >=
    P_EQ,       // == !=
    P_LP,       // (
    P_RP,       // )
    P_ID,       // operand (i)
    P_END,      // $
    P_TERM_COUNT,
    SYM_E = P_TERM_COUNT,   // neterminal E
    SYM_HANDLE              // zarazka <
};

//precednci tabulka
//F konec, < odlozit , > redukovat, = uzavirka, ' 'chyba
//radek nejvyssi terminal, sloupec co dojde
static const char PSA_TABLE[P_TERM_COUNT][P_TERM_COUNT]= {
    //      UN  MUL  ADD  REL  EQ    (    )    I    DNO 
           {'<','>', '>', '>', '>', '<', '>', '<',  '>' },//UN
           {'<','>', '>', '>', '>', '<', '>', '<',  '>' },//MUL
           {'<','<', '>', '>', '>', '<', '>', '<',  '>' },//ADD
           {'<','<', '<', ' ', '>', '<', '>', '<',  '>' },// REL 
           {'<','<', '<', '<', ' ', '<', '>', '<',  '>' },// EQ 
           {'<','<', '<', '<', '<', '<', '=', '<',  ' ' },// (                                             
           {' ','>', '>', '>', '>', ' ', '>', ' ',  '>' },// )                                           
           {' ','>', '>', '>', '>', ' ', '>', ' ',  '>' },// I                                             
           {'<','<', '<', '<', '<', '<', ' ', '<',  'F' },//DNO                                            
};


int parse_expression(tree_t *global_symtable, tree_t *local_symtable, data_type_t *out_type, bool is_condition){


    if(current_token.type== TOKEN_EOL 
        || current_token.type ==TOKEN_EOF
        || current_token.type == TOKEN_SEMICOLON  
        || current_token.type == KEYWORD_THEN  
        || current_token.type == KEYWORD_DO  
        || current_token.type == TOKEN_RPAREN  
        || current_token.type ==TOKEN_COMMA){
            return 2; // vyraz nesmi byt prazdny
        }

    while (current_token.type != TOKEN_EOL 
        && current_token.type !=TOKEN_EOF
        && current_token.type != TOKEN_SEMICOLON  
        && current_token.type != KEYWORD_THEN  
        && current_token.type != KEYWORD_DO  
        && current_token.type != TOKEN_RPAREN  
        && current_token.type !=TOKEN_COMMA)
    {
        next();
    }
    
    *out_type = TYPE_UNIT;
    return 0;
}

int parse_expression_ex(tree_t *global_symtable, tree_t *local_symtable, data_type_t *out_type, ast_node_t **out_node,bool is_condition, bool in_parens){

    }