 #include "expr_parser.h"
 #include "parser.h"



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