#include "parser.h"
#include "automat.h"
#include "symtable.h"
#include "expr_parser.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

//pomocne struktry, funkce
typedef struct {
    char *func_name;
    data_type_t *arg_types;
    int arg_count;
} pending_call_t;

typedef struct {
    pending_call_t *items;
    int count;
    int capacity;
} pending_call_list_t;

static pending_call_list_t pending_calls = {NULL, 0, 0};


static void pending_calls_free(void);
static int verify_pending_calls(tree_t *global_symtable);
static int expr_ended(void);
static int parse_val_decl(tree_t *global_symtable, tree_t *local_symtable);
static int parse_function_call(tree_t *global_symtable, tree_t *local_symtable, data_type_t *out_return_type);
static int parse_assignment(tree_t *global_symtable, tree_t *local_symtable);
static int parse_if(tree_t *global_symtable, tree_t *local_symtable, indent_stack_t *indent_stack, data_type_t func_return_type);
static int parse_while(tree_t *global_symtable, tree_t *local_symtable, indent_stack_t *indent_stack, data_type_t func_return_type);
static int parse_return(tree_t *global_symtable, tree_t *local_symtable, data_type_t func_return_type);


//pomocne fce pro parsovani bloku
static int indent_top(const indent_stack_t *stack){
    return stack->items[stack->top];
}

static int indent_push(indent_stack_t *stack,int col){
    if (stack->top + 1 >= stack->capacity)
    {
        int new_cap = stack ->capacity +2;
        int *tmp = realloc(stack->items,new_cap *sizeof(int));
        if(tmp == NULL){
            return 99;
        }
        stack ->items = tmp;
        stack ->capacity = new_cap;
    }
    stack -> items[++stack ->top] = col;
    return 0;
    
}
//kontrola jestli po zmenseni odasazeni jsem na stejne urovni jako nejaky blok
static bool indent_contains(const indent_stack_t *stack,int col){
    for(int i = 0; i <=stack ->top;i++){
        if(stack ->items[i] ==col){
            return true;
        }
    }
    return false;
}

static void indent_pop(indent_stack_t *stack){
    if(stack ->top > 0){
        stack -> top--;
    }
}

static int indent_init(indent_stack_t *stack) {
    stack->capacity = 8;
    stack->top = 0;
    stack->items = malloc(stack->capacity * sizeof(int));
    if (stack->items == NULL) {
        return 99;
    }
    stack->items[0] = 1; 
    return 0;
}

static void indent_free(indent_stack_t *stack) {
    if (stack->items != NULL) {
        free(stack->items);
        stack->items = NULL;
    }
    stack->top = 0;
    stack->capacity = 0;
}

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
    return ahead_token;
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

static void cleanup_parser(tree_t *global_symtable,indent_stack_t *indent_stack ){
    if (indent_stack != NULL) {
        indent_free(indent_stack);
    }
    free_token(&current_token);
    if (looked_ahead) {
        free_token(&ahead_token);
        looked_ahead = false;
    }
    if (global_symtable != NULL) {
        symtable_destroy_tree(global_symtable);
    }
    pending_calls_free();
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

    if(current_token.type != KEYWORD_IMPORT || current_token.col != 1){
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
    pending_calls.items = NULL; // reset 
    pending_calls.count = 0;
    pending_calls.capacity = 0;
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
        cleanup_parser(global_symtable,&indent_stack);
        return 2; // CHYBA // Chyba v prologu
    }

    while (current_token.type != TOKEN_EOF) {
        switch (current_token.type) {
            case KEYWORD_DEF:
        
                result = parse_function(global_symtable, &indent_stack);
                if (result != 0){
                    cleanup_parser(global_symtable,&indent_stack);
                    return result; // CHYBA
                } 
                break;
                
            case TOKEN_EOL:
                
                next();
                break;
            
            case TOKEN_AT_MAIN:

                if(current_token.col != indent_top(&indent_stack)){
                    cleanup_parser(global_symtable,&indent_stack);
                    return 2;
                }
                next();
                bool had_EOL = false;          
                while (current_token.type == TOKEN_EOL){ 
                    next(); 
                    had_EOL = true;
                }

                if(current_token.type != KEYWORD_DEF || (had_EOL && indent_top(&indent_stack)!= current_token.col)){
                    cleanup_parser(global_symtable,&indent_stack);
                    return 2; // CHYBA 
                }

                if(main) {
                    cleanup_parser(global_symtable,&indent_stack);
                    return 4; // CHYBA vice mainu
                }
                main = 1;

                result = parse_main(global_symtable,&indent_stack);
                if (result != 0) {
                    cleanup_parser(global_symtable,&indent_stack);
                    return result; //CHYBA
                }

                break;

            default:
                // Neočekávaný token
                cleanup_parser(global_symtable,&indent_stack);
                return 2; // CHYBA // Syntaktická chyba
        }
    }

    result = verify_pending_calls(global_symtable);
    if (result != 0) {
        cleanup_parser(global_symtable, &indent_stack);
        return result;
    }

    if(main != 1){
        cleanup_parser(global_symtable,&indent_stack);
        return 3; // CHYBA 
    }
    cleanup_parser(global_symtable,&indent_stack);
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
        if(symtable_lookup_local(local_symtable, current_token.str_val) != NULL){
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
        next(); // za jmeno
        while(current_token.type == TOKEN_EOL){
            next();
        }
        if(current_token.type != TOKEN_COLON){
            free(*out_param_types);
            free(param_name);
            *out_param_types = NULL;
            return 2;
        }
 
        next(); // typ parametru
        while(current_token.type == TOKEN_EOL){
            next();
        }
        data_type_t param_type;
 
        int type_res = parse_type(&param_type);
        if(type_res != 0){
            free(*out_param_types);
            free(param_name);
            *out_param_types = NULL;
            return type_res; // v type res je ta chyba ktera se stala
        }
 
        int ins_res = symtable_insert(local_symtable, param_name,PARAM,param_type,param_line);
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
 
        while(current_token.type == TOKEN_EOL){
             next();
        }
 
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

static bool is_type_compatible(data_type_t source, data_type_t target){
    if(target == source) {
        return true;
    }
    if(target == TYPE_ANY || source == TYPE_ANY){
        return true;
    }
    if(target == TYPE_DOUBLE && source == TYPE_INT){
        return true;
    }
    return false;
}

static int parse_statement(tree_t *global_symtable, tree_t *local_symtable, indent_stack_t *indent_stack, data_type_t func_return_type){
   int res;
   
   switch (current_token.type)
   {
    case KEYWORD_VAL:
    case KEYWORD_VAR:
        res = parse_val_decl(global_symtable,local_symtable);
        return (res!= 0) ? res :expr_ended();
    case TOKEN_IDENTIFIER:{
        token_t next_token = peek_token();
        if(next_token.type == TOKEN_LPAREN){
            data_type_t tmp_ret;
            res = parse_function_call(global_symtable,local_symtable,&tmp_ret);
            
        }else if(next_token.type == TOKEN_ASSIGN){
            res = parse_assignment(global_symtable,local_symtable);

        }else{
            data_type_t tmp_expr;
            res = parse_expression(global_symtable,local_symtable,&tmp_expr,false);
        }
        return (res != 0) ? res : expr_ended();
    }
    case KEYWORD_IF:
        return parse_if(global_symtable,local_symtable,indent_stack,func_return_type);
    case KEYWORD_WHILE:
        return parse_while(global_symtable,local_symtable,indent_stack,func_return_type);
    case KEYWORD_RETURN:
        res = parse_return(global_symtable,local_symtable,func_return_type);
        return (res != 0) ? res : expr_ended();
    default:{
        data_type_t tmp_exp;
        res = parse_expression(global_symtable,local_symtable,&tmp_exp,false);
        return (res != 0) ? res : expr_ended();    
    }
   }
}

//kontorla ze jednoradkovy prikaz skoncil dobre
static int expr_ended(){
    if(current_token.type != TOKEN_EOL && current_token.type != TOKEN_EOF && current_token.type != TOKEN_SEMICOLON){
        return 2;
    }
    return 0;
}

static void pending_calls_free(void) {
    for (int i = 0; i < pending_calls.count; i++) {
        free(pending_calls.items[i].func_name);
        free(pending_calls.items[i].arg_types);
    }
    free(pending_calls.items);
    pending_calls.items = NULL;
    pending_calls.count = 0;
    pending_calls.capacity = 0;
}

static int pending_calls_add(const char *func_name, const data_type_t *arg_types, int arg_count) {
    if (pending_calls.count >= pending_calls.capacity) {
        int new_cap = (pending_calls.capacity == 0) ? 8 : pending_calls.capacity * 2;
        pending_call_t *tmp = realloc(pending_calls.items, new_cap * sizeof(pending_call_t));
        if (tmp == NULL) {
            return 99;
        }
        pending_calls.items = tmp;
        pending_calls.capacity = new_cap;
    }

    char *name_copy = malloc(strlen(func_name) + 1);
    if (name_copy == NULL) {
        return 99;
    }
    strcpy(name_copy, func_name);

    data_type_t *types_copy = NULL;
    if (arg_count > 0) {
        types_copy = malloc(arg_count * sizeof(data_type_t));
        if (types_copy == NULL) {
            free(name_copy);
            return 99;
        }
        memcpy(types_copy, arg_types, arg_count * sizeof(data_type_t));
    }
    pending_calls.items[pending_calls.count].func_name = name_copy;
    pending_calls.items[pending_calls.count].arg_types = types_copy;
    pending_calls.items[pending_calls.count].arg_count = arg_count;
    pending_calls.count++;
    return 0;
}

static int verify_pending_calls(tree_t *global_symtable) {
    for (int i = 0; i < pending_calls.count; i++) {
        pending_call_t *call = &pending_calls.items[i];
        tdata_t *func_sym = symtable_lookup(global_symtable, call->func_name);
        if (func_sym == NULL || func_sym->type != FUNCTION) {
            return 3; // undefined funkce
        }
        if (call->arg_count != func_sym->param_count) {
            return 5; //  pocet arg
        }
        for (int j = 0; j < call->arg_count; j++) {
            if (!is_type_compatible(call->arg_types[j], func_sym->param_types[j])) {
                return 5; //  typ arg
            }
        }
        if(func_sym->data_type != TYPE_ANY){
            return 6; // neodvoditelny typ
        }
    }
    return 0;
}

static int parse_function_call(tree_t *global_symtable, tree_t *local_symtable, data_type_t *out_return_type) {
    if (current_token.type != TOKEN_IDENTIFIER) {
        return 2;
    }
    char *func_name = malloc(strlen(current_token.str_val) + 1);
    if (func_name == NULL) {
        return 99;
    }
    strcpy(func_name, current_token.str_val);

    next(); // na (
    if (current_token.type != TOKEN_LPAREN) {
        free(func_name);
        return 2;
    }
    next(); // za (

    while (current_token.type == TOKEN_EOL) {
        next();
    }

    data_type_t *arg_types = NULL;
    int arg_count = 0;

    if (current_token.type != TOKEN_RPAREN) {
        while (true) {
            data_type_t arg_type;
            int res = parse_expression(global_symtable, local_symtable, &arg_type, false);
            if (res != 0) {
                free(func_name);
                free(arg_types);
                return res;
            }

            data_type_t *tmp = realloc(arg_types, (arg_count + 1) * sizeof(data_type_t));
            if (tmp == NULL) {
                free(func_name);
                free(arg_types);
                return 99;
            }
            arg_types = tmp;
            arg_types[arg_count++] = arg_type;

            if (current_token.type == TOKEN_COMMA) {
                next();
                bool eol = false;
                while (current_token.type == TOKEN_EOL) { 
                    eol = true; next(); 
                }
                if (current_token.type == TOKEN_RPAREN) {
                    if (!eol){ 
                        free(func_name); 
                        free(arg_types); 
                        return 2; 
                    }
                    break;
                }
            } else {
                while (current_token.type == TOKEN_EOL) {
                    next();
                }
                if (current_token.type == TOKEN_RPAREN) {
                    break;
                }
                free(func_name);
                free(arg_types);
                return 2;
            }
        }
    }

    next(); // za )


    tdata_t *func_sym = symtable_lookup(global_symtable, func_name);
    if (func_sym != NULL) {
        if (func_sym->type != FUNCTION) {
            free(func_name);
            free(arg_types);
            return 3;
        }
        if (func_sym->data_type == TYPE_ANY) { 
            free(func_name); 
            free(arg_types); 
            return 6; 
        }
        if (arg_count != func_sym->param_count) {
            free(func_name);
            free(arg_types);
            return 5;
        }
        for (int i = 0; i < arg_count; i++) {
            if (!is_type_compatible(arg_types[i], func_sym->param_types[i])) {
                free(func_name);
                free(arg_types);
                return 5;
            }
        }
        *out_return_type = func_sym->data_type;
    } else {
        // funkce jeste nebyla defined
        int add_res = pending_calls_add(func_name, arg_types, arg_count);
        if (add_res != 0) {
            free(func_name);
            free(arg_types);
            return add_res;
        }
        *out_return_type = TYPE_ANY;
    }

    free(func_name);
    free(arg_types);
    return 0;
}
//prava strana za =/return
static int parse_rs(tree_t *global_symtable, tree_t *local_symtable, data_type_t *out_type){
    if(current_token.type == TOKEN_IDENTIFIER && peek_token().type == TOKEN_LPAREN){
        return parse_function_call(global_symtable,local_symtable,out_type);
    }
    return parse_expression(global_symtable,local_symtable,out_type,false);
}

//deklareace val/var

static int parse_val_decl(tree_t *global_symtable,tree_t *local_symtable){
    bool is_const = (current_token.type == KEYWORD_VAL);

    next(); // na ID

    if(current_token.type != TOKEN_IDENTIFIER){
        return 2;
    }

    //jen lokalni uvnitr to muzu prejmenovat atd
    if(symtable_lookup_local(local_symtable,current_token.str_val) != NULL){
        return 4;
    }

    char *var_name = malloc(strlen(current_token.str_val)+1);
    if(var_name == NULL){
        return 99;
    }   
    strcpy(var_name,current_token.str_val);


    int var_line = current_token.line;

    next(); // za jmeno 


    bool has_type = false;
    data_type_t decl_type = TYPE_UNIT;
    if(current_token.type == TOKEN_COLON){
        next();//za :

        int type_res = parse_type(&decl_type);
        if(type_res != 0){
            free(var_name);
            return type_res;
        }
        if(decl_type == TYPE_UNIT){
            free(var_name);
            return 6; //promena nemuze byt TYPEUNIT
        }
        has_type = true;
    }

    if(current_token.type != TOKEN_ASSIGN){
        free(var_name);
        return 2;
    }

    next(); // za =

    data_type_t rs_type;

    int rs_res = parse_rs(global_symtable,local_symtable,&rs_type);
    if(rs_res != 0){
        free(var_name);
        return rs_res;
    }

    data_type_t final_type;

    if(has_type){
        if(!is_type_compatible(rs_type,decl_type)){
            free(var_name);
            return 6;
        }
        final_type = decl_type;
    }else{
        if(rs_type == TYPE_UNIT){
            free(var_name);
            return 6;
        }
        final_type = rs_type;
    }

    int ins_res = symtable_insert(local_symtable,var_name,is_const ? KONSTANT:VARIABLE, final_type,var_line);
    free(var_name);
    if(ins_res!=0){
        return 99;
    }
    return 0;

}   

//prirazeni do existujici promenne
static int parse_assignment(tree_t *global_symtable, tree_t *local_symtable){
    tdata_t *var_sym = symtable_lookup(local_symtable,current_token.str_val);
    if(var_sym == NULL || var_sym->type == FUNCTION){
        return 3; //undefined
    }
    if(var_sym -> type !=VARIABLE){
        return 7; //pokus o zapis na spatne misto
    }

    next();// na =

    if(current_token.type != TOKEN_ASSIGN){
        return 2;
    }
    next(); //za =

    data_type_t rs_type;
    int rs_res = parse_rs(global_symtable,local_symtable,&rs_type);
    if(rs_res != 0){
        return rs_res;
    }


    if(!is_type_compatible(rs_type,var_sym->data_type)){
        return 6;
    }
    return 0;
}


static int parse_if(tree_t *global_symtable, tree_t *local_symtable,indent_stack_t *indent_stack, data_type_t func_return_type){
    int if_col = indent_top(indent_stack);//odsazeni if
    next();//za if

    data_type_t cond_type;
    int res = parse_expression(global_symtable,local_symtable,&cond_type,true);
    if(res!=0){
        return res;
    }
    if(current_token.type != KEYWORD_THEN){
        return 2; 
    }

    next(); //za then

    if(current_token.type != TOKEN_EOL){
        return 2;
    }

    tree_t *then_symtable = symtable_init(local_symtable);
    if(then_symtable == NULL){
        return 99;
    }

    res = parse_block(global_symtable,then_symtable,indent_stack,func_return_type);
    symtable_destroy_tree(then_symtable);
    if(res != 0){
        return res;
    }

    while (current_token.type == TOKEN_EOL)
    {
        next();
    }
    

    if(current_token.type != KEYWORD_ELSE || current_token.col != if_col){
        return 2;
    }
    next(); //za else


    if(current_token.type != TOKEN_EOL){
        return 2;
    }

    tree_t *else_symtable = symtable_init(local_symtable);
    if(else_symtable == NULL){
        return 99;
    }

    res = parse_block(global_symtable,else_symtable,indent_stack,func_return_type);
    symtable_destroy_tree(else_symtable);
    
    return res;
}



static int parse_while(tree_t *global_symtable, tree_t *local_symtable,indent_stack_t *indent_stack, data_type_t func_return_type){

    next(); // za while
    data_type_t cond_type;
    int res = parse_expression(global_symtable,local_symtable,&cond_type,true);
    if(res!=0){
        return res;
    }
    if(current_token.type != KEYWORD_DO){
        return 2;
    }
    next();
    if(current_token.type != TOKEN_EOL){
        return 2;
    }

    tree_t *while_symtable = symtable_init(local_symtable);
    if(while_symtable == NULL){
        return 99;
    }
    res = parse_block(global_symtable,while_symtable,indent_stack,func_return_type);
    symtable_destroy_tree(while_symtable);
    return res;
}


static int parse_return(tree_t *global_symtable, tree_t *local_symtable, data_type_t func_return_type){
    next(); // za return
    if(func_return_type == TYPE_ANY){
        return 6; // musi byt explicitni typ
    }
    if (current_token.type == TOKEN_EOL || current_token.type == TOKEN_SEMICOLON || current_token.type == TOKEN_EOF) {
        if(func_return_type != TYPE_UNIT){
            return 6;
        }
        return 0;
    }   

    if(func_return_type == TYPE_UNIT){
        return 6;
    }

    data_type_t expr_type;
    int res=parse_rs(global_symtable,local_symtable,&expr_type);
    if(res!= 0){
        return res;
    }

    if(!is_type_compatible(expr_type,func_return_type) && func_return_type != TYPE_UNIT){
        return 6;
    }
    return 0;

}


int parse_block(tree_t *global_symtable, tree_t *local_symtable, indent_stack_t *indent_stack, data_type_t func_return_type){

    //preskoceni prazdnych
    while(current_token.type == TOKEN_EOL){
        next();
    }

    if(current_token.type == TOKEN_EOF){
        return 2;
    }
    //prvni prikaz v bloku musi byt vys nez nadraena fce
    int block_col = current_token.col;
    if(block_col <= indent_top(indent_stack)){
        return 2;
    }


    if(indent_push(indent_stack,block_col) != 0){
        return 99;
    }

    while(current_token.type != TOKEN_EOF){
        if(current_token.col <block_col){
            indent_pop(indent_stack);
            if(!indent_contains(indent_stack,current_token.col)){
                return 2; //spatne odsazeni
            }

            return 0;
        }
        if(current_token.col > block_col){
            indent_pop(indent_stack);
            return 2; // zvetseni odsazeni uprostred bloku
        }

        int res = parse_statement(global_symtable,local_symtable,indent_stack,func_return_type);
        if(res != 0){
            indent_pop(indent_stack);
            return res;
        }
        //prikazy na jedom radku
        while(current_token.type == TOKEN_SEMICOLON){
            next(); 
            if(current_token.type ==TOKEN_SEMICOLON){
                indent_pop(indent_stack);
                return 2; //2 ; za sebou
            }
            if(current_token.type == TOKEN_EOL || current_token.type == TOKEN_EOF){
                break; //strednik na konci radku
            }

            res = parse_statement(global_symtable,local_symtable,indent_stack,func_return_type);
            if(res != 0){
                indent_pop(indent_stack);
                return res;
            }
        }
        while (current_token.type == TOKEN_EOL)
        {
            next();
        }

    }
    indent_pop(indent_stack);
    return 0;
}

int parse_function(tree_t *global_symtable, indent_stack_t *indent_stack) {

    
    if(current_token.col != indent_top(indent_stack)){
        return 2;
    }
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
    data_type_t return_type = TYPE_ANY;
    bool explicit_type = false;
    if(current_token.type == TOKEN_COLON){
        next(); //za :
        res = parse_type(&return_type);
        if (res != 0) {
            free(func_name);
            free(param_types);
            symtable_destroy_tree(local_symtable);
            return res;
        }
        explicit_type = true;
    }

    int ins_res = symtable_insert_func(global_symtable, func_name,return_type,func_line,param_count,param_types);
    tdata_t *fn = symtable_lookup(global_symtable,func_name);
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

    next();

    if(current_token.type == TOKEN_EOL){
        res = parse_block(global_symtable,local_symtable,indent_stack, return_type);
        if (res != 0){
            symtable_destroy_tree(local_symtable);
            return res;
        }
    }else if(current_token.type == TOKEN_EOF){
        symtable_destroy_tree(local_symtable);
        return 2; // funkce nema telo
    }else {
        //dodelat jednoradkove telo funkce

        data_type_t body_type;
        res = parse_rs(global_symtable,local_symtable,&body_type);
        if (res != 0){
            symtable_destroy_tree(local_symtable);
            return res;
        }

        if(explicit_type){
            // v Unit funkci se zahazuje, jinak musi byt dobre
            if(return_type != TYPE_UNIT && !is_type_compatible(body_type, return_type)){
                symtable_destroy_tree(local_symtable);
                return 6;
            }
        }else{
            if(body_type == TYPE_ANY){        // typ nelze odvodit
                symtable_destroy_tree(local_symtable);
                return 6;
            }
            fn->data_type = body_type;
        }

        if (current_token.type  != TOKEN_EOL && current_token.type != TOKEN_EOF){
            symtable_destroy_tree(local_symtable);
            return 2;
        }
        
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
        if(symtable_insert(global_symtable, current_token.str_val, FUNCTION, TYPE_UNIT, current_token.line) != 0){
            return 99;
        }
        
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
    int res = 0;

    if(current_token.type == TOKEN_EOL){
        res = parse_block(global_symtable,local_symtable,indent_stack,TYPE_UNIT);
        if (res != 0){
            symtable_destroy_tree(local_symtable);
            return res;
        }
    }else if(current_token.type == TOKEN_EOF){
        symtable_destroy_tree(local_symtable);
        return 2; // funkce nema telo
    }else {
        //dodelat jednoradkove telo funkce

        data_type_t body_type;
        res = parse_rs(global_symtable,local_symtable,&body_type);
        if (res == 0){
            if(!is_type_compatible(body_type,TYPE_UNIT)){
                res =  6;
            }else if(current_token.type  != TOKEN_EOL && current_token.type != TOKEN_EOF){
                res = 2;
            }
        }
    }
    symtable_destroy_tree(local_symtable);
    return res; 
}

int register_functions(tree_t *global_symtable){

    data_type_t p_any[] = {TYPE_ANY};
    data_type_t p_double[] = {TYPE_DOUBLE};
    data_type_t p_string[] = {TYPE_STRING};
    data_type_t p_int[] = {TYPE_INT};
    data_type_t p_substr[] = {TYPE_STRING,TYPE_INT,TYPE_INT};
    data_type_t p_ord[] = {TYPE_STRING,TYPE_INT};

    int control = 0;
    control |= symtable_insert_func(global_symtable, "print",      TYPE_UNIT,   0, 1, p_any);
    control |= symtable_insert_func(global_symtable, "readLine",   TYPE_STRING, 0, 0, NULL);
    control |= symtable_insert_func(global_symtable, "readInt",    TYPE_INT,    0, 0, NULL);
    control |= symtable_insert_func(global_symtable, "readDouble", TYPE_DOUBLE, 0, 0, NULL);
    control |= symtable_insert_func(global_symtable, "toStr",      TYPE_STRING, 0, 1, p_any);
    control |= symtable_insert_func(global_symtable, "toInt",      TYPE_INT,    0, 1, p_double);
    control |= symtable_insert_func(global_symtable, "strLength",  TYPE_INT,    0, 1, p_string);
    control |= symtable_insert_func(global_symtable, "subStr",     TYPE_STRING, 0, 3, p_substr);
    control |= symtable_insert_func(global_symtable, "ord",        TYPE_INT,    0, 2, p_ord);
    control |= symtable_insert_func(global_symtable, "chr",        TYPE_STRING, 0, 1, p_int);

    if(control != 0 ){
        return 99;
    }   
    return 0;
}
